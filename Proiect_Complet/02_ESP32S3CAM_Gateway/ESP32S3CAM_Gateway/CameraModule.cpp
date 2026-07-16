#include "CameraModule.h"
#include "Config.h"
#include "esp_camera.h"
#include "esp_http_server.h"

bool cameraReady = false;
// Serverul HTTP separat pentru fluxul video
static httpd_handle_t streamHttpd = nullptr;

//fluxul video
static esp_err_t streamHandler(httpd_req_t* req) {
	static const char* CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";
	static const char* BOUNDARY = "\r\n--frame\r\n";
	static const char* PART = "Content-Type: image/jpeg\r\n"
	"Content-Length: %u\r\n\r\n";

	esp_err_t result = httpd_resp_set_type(req, CONTENT_TYPE);
	if (result != ESP_OK) {
		return result;
	}

	// Permite accesarea fluxului din aplicația web
	httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
	char header[64];
	while (true) {

		camera_fb_t* fb = esp_camera_fb_get();
		if (!fb) {
			return ESP_FAIL;
		}

		const size_t headerLen = snprintf(header, sizeof(header), PART, fb->len);
		result = httpd_resp_send_chunk(req, BOUNDARY, strlen(BOUNDARY));

		if (result == ESP_OK) {
			result = httpd_resp_send_chunk(req, header, headerLen);
		}

		if (result == ESP_OK) {
			result = httpd_resp_send_chunk(
			req, reinterpret_cast<const char*>(fb->buf), fb->len);
		}

		esp_camera_fb_return(fb);
		if (result != ESP_OK) {
			break;
	}
	delay(1);
}
return result;
}

bool initCamera() {
	camera_config_t config = {};
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sccb_sda = SIOD_GPIO_NUM;
	config.pin_sccb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 15000000;
	config.pixel_format = PIXFORMAT_JPEG;
	config.grab_mode = CAMERA_GRAB_LATEST;

	const bool hasPsram = psramFound();
	if (hasPsram) {
		config.frame_size = FRAMESIZE_VGA;
		config.jpeg_quality = 12;
		config.fb_count = 2;
		config.fb_location = CAMERA_FB_IN_PSRAM;
	} else {
		config.frame_size = FRAMESIZE_QVGA;
		config.jpeg_quality = 15;
		config.fb_count = 1;
		config.fb_location = CAMERA_FB_IN_DRAM;
	}

	const esp_err_t err = esp_camera_init(&config);
	if (err != ESP_OK) {
		Serial.printf("[CAMERA] Eroare initializare: 0x%x\n", err);
		cameraReady = false;
		return false;
	}

	sensor_t* sensor = esp_camera_sensor_get();
	if (sensor) {
		sensor->set_vflip(sensor, 1);
		sensor->set_framesize(sensor, hasPsram ? FRAMESIZE_VGA : FRAMESIZE_QVGA);
	}
	cameraReady = true;
	return true;
}

//server video
bool startStreamServer() {
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	// Server separat de controlul HTTP principal
	config.server_port = 81;
	// Evită conflictul cu serverul HTTP de control
	config.ctrl_port += 1;
	config.max_open_sockets = 4;
  
	httpd_uri_t streamUri = {};
	streamUri.uri = "/stream";
	streamUri.method = HTTP_GET;
	streamUri.handler = streamHandler;
	streamUri.user_ctx = nullptr;

	if (httpd_start(&streamHttpd, &config) != ESP_OK) {
		Serial.println("[STREAM] Eroare la pornirea serverului");
		return false;
	}

	if (httpd_register_uri_handler(streamHttpd, &streamUri) != ESP_OK) {
		Serial.println("[STREAM] Eroare la inregistrarea rutei /stream");
		return false;
	}
	Serial.println("[STREAM] Server video pornit pe portul 81");
	return true;
}