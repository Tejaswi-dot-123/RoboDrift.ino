// en: Install "WebSockets" by Markus Sattler via Arduino IDE Library Manager. Arduino.h, WiFi.h and WiFiClient.h come pre-installed, no setup needed.
// tr: Arduino IDE Kütüphane Yöneticisi'nden Markus Sattler'ın "WebSockets" kütüphanesini kurun. Arduino.h, WiFi.h ve WiFiClient.h önceden yüklü gelir, kurulum gerekmez.
// es: Instale "WebSockets" de Markus Sattler desde el Gestor de Librerías de Arduino IDE. Arduino.h, WiFi.h y WiFiClient.h vienen preinstaladas, no requieren configuración.
// pt-PT: Instale "WebSockets" de Markus Sattler pelo Gestor de Bibliotecas do Arduino IDE. Arduino.h, WiFi.h e WiFiClient.h vêm pré-instaladas, não requerem configuração.
// pt-BR: Instale "WebSockets" de Markus Sattler pelo Gerenciador de Bibliotecas do Arduino IDE. Arduino.h, WiFi.h e WiFiClient.h vêm pré-instaladas, não requerem configuração.
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>

// en: Enter your WiFi network name (SSID) and password in the fields below.
// tr: Aşağıdaki alanlara WiFi ağ adınızı (SSID) ve şifrenizi giriniz.
// es: Introduzca el nombre de su red WiFi (SSID) y la contraseña en los campos a continuación.
// pt-PT: Introduza o nome da sua rede WiFi (SSID) e a palavra-passe nos campos abaixo.
// pt-BR: Digite o nome da sua rede WiFi (SSID) e a senha nos campos abaixo.
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

unsigned long lastCommandTime = 0;
const unsigned long TIMEOUT = 500;
bool isStopped = false;

// en: WebSocket port is set to 81. The same port must be entered in the app. If you change it, update the app as well.
// tr: WebSocket portu 81 olarak ayarlanmıştır. Aynı port uygulamaya da girilmelidir. Değiştirirseniz uygulamayı da güncelleyin.
// es: El puerto WebSocket está configurado en 81. El mismo puerto debe introducirse en la app. Si lo cambia, actualícelo también en la app.
// pt-PT: A porta WebSocket está definida como 81. A mesma porta deve ser introduzida na app. Se a alterar, atualize também na app.
// pt-BR: A porta WebSocket está definida como 81. A mesma porta deve ser inserida no app. Se alterá-la, atualize também no app.
WebSocketsServer webSocket = WebSocketsServer(81);

// en: Channel A = RIGHT motor (ENA_RIGHT, IN1_RIGHT, IN2_RIGHT) | Channel B = LEFT motor (ENB_LEFT, IN3_LEFT, IN4_LEFT). If motors spin in wrong direction, swap IN1/IN2 or IN3/IN4 connections.
// tr: Kanal A = SAĞ motor (ENA_RIGHT, IN1_RIGHT, IN2_RIGHT) | Kanal B = SOL motor (ENB_LEFT, IN3_LEFT, IN4_LEFT). Motorlar ters yönde dönüyorsa IN1/IN2 veya IN3/IN4 bağlantılarını yer değiştirin.
// es: Canal A = Motor DERECHO (ENA_RIGHT, IN1_RIGHT, IN2_RIGHT) | Canal B = Motor IZQUIERDO (ENB_LEFT, IN3_LEFT, IN4_LEFT). Si los motores giran en dirección incorrecta, intercambie las conexiones IN1/IN2 o IN3/IN4.
// pt-PT: Canal A = Motor DIREITO (ENA_RIGHT, IN1_RIGHT, IN2_RIGHT) | Canal B = Motor ESQUERDO (ENB_LEFT, IN3_LEFT, IN4_LEFT). Se os motores girarem na direção errada, troque as ligações IN1/IN2 ou IN3/IN4.
// pt-BR: Canal A = Motor DIREITO (ENA_RIGHT, IN1_RIGHT, IN2_RIGHT) | Canal B = Motor ESQUERDO (ENB_LEFT, IN3_LEFT, IN4_LEFT). Se os motores girarem na direção errada, troque as conexões IN1/IN2 ou IN3/IN4.
#define ENA_RIGHT   14
#define IN1_RIGHT   26
#define IN2_RIGHT   27
#define ENB_LEFT    25
#define IN3_LEFT    32
#define IN4_LEFT    33

// en: General purpose digital output pins. Assign functions as needed for your design.
// tr: Genel amaçlı dijital çıkış pinleri. Tasarımınıza uygun işlev atayabilirsiniz.
// es: Pines de salida digital de propósito general. Asigne funciones según su diseño.
// pt-PT: Pinos de saída digital de uso geral. Atribua funções conforme o seu design.
// pt-BR: Pinos de saída digital de uso geral. Atribua funções conforme o seu design.
#define F1_PIN  18
#define F2_PIN  19
#define F3_PIN  23

// en: PWM channel definitions for ESP32 (ESP32 uses LEDC instead of analogWrite)
// tr: ESP32 PWM kanal tanımlamaları (ESP32, analogWrite yerine LEDC kullanır)
// es: Definiciones de canal PWM para ESP32 (ESP32 usa LEDC en lugar de analogWrite)
// pt-PT: Definições de canal PWM para ESP32 (ESP32 usa LEDC em vez de analogWrite)
// pt-BR: Definições de canal PWM para ESP32 (ESP32 usa LEDC em vez de analogWrite)
#define ENA_RIGHT_CHANNEL  0
#define ENB_LEFT_CHANNEL   1
#define PWM_FREQ           1000
#define PWM_RESOLUTION     8

int currentSpeed = 153;

const int speedTable[11] = {0, 25, 51, 76, 102, 127, 153, 178, 204, 229, 255};

bool f1State = false;
bool f2State = false;
bool f3State = false;

// en: IMPORTANT: This code uses ESP32 board package v3.x.x. In v3.x.x, ledcWrite uses PIN number, not channel number. If you use v2.x.x, replace ledcWrite(PIN, speed) with ledcWrite(CHANNEL, speed).
// tr: ÖNEMLİ: Bu kod ESP32 board paketi v3.x.x için yazılmıştır. v3.x.x'te ledcWrite kanal numarası değil PIN numarası kullanır. v2.x.x kullanıyorsanız ledcWrite(PIN, hız) yerine ledcWrite(KANAL, hız) kullanın.
// es: IMPORTANTE: Este código usa el paquete ESP32 v3.x.x. En v3.x.x, ledcWrite usa el número de PIN, no el canal. Si usa v2.x.x, reemplace ledcWrite(PIN, velocidad) por ledcWrite(CANAL, velocidad).
// pt-PT: IMPORTANTE: Este código usa o pacote ESP32 v3.x.x. Na v3.x.x, ledcWrite usa o número do PIN, não o canal. Se usar v2.x.x, substitua ledcWrite(PIN, velocidade) por ledcWrite(CANAL, velocidade).
// pt-BR: IMPORTANTE: Este código usa o pacote ESP32 v3.x.x. Na v3.x.x, ledcWrite usa o número do PIN, não o canal. Se usar v2.x.x, substitua ledcWrite(PIN, velocidade) por ledcWrite(CANAL, velocidade).
void setMotor(bool rightFwd, bool rightBack, int rightSpeed,
              bool leftFwd, bool leftBack, int leftSpeed) {
    digitalWrite(IN1_RIGHT, leftFwd);
    digitalWrite(IN2_RIGHT, leftBack);
    ledcWrite(ENA_RIGHT, leftSpeed);
    digitalWrite(IN3_LEFT, rightFwd);
    digitalWrite(IN4_LEFT, rightBack);
    ledcWrite(ENB_LEFT, rightSpeed);
}

void stopMotors() {
    setMotor(false, false, 0, false, false, 0);
    isStopped = true;
}

void moveForward()       { setMotor(true, false, currentSpeed, true, false, currentSpeed); }
void moveBackward()      { setMotor(false, true, currentSpeed, false, true, currentSpeed); }
void turnLeft()          { setMotor(true, false, currentSpeed, false, true, currentSpeed); }
void turnRight()         { setMotor(false, true, currentSpeed, true, false, currentSpeed); }
void moveForwardRight()  { setMotor(true, false, currentSpeed, true, false, currentSpeed / 2); }
void moveForwardLeft()   { setMotor(true, false, currentSpeed / 2, true, false, currentSpeed); }
void moveBackwardRight() { setMotor(false, true, currentSpeed, false, true, currentSpeed / 2); }
void moveBackwardLeft()  { setMotor(false, true, currentSpeed / 2, false, true, currentSpeed); }

void handleCommand(String cmd) {
    lastCommandTime = millis();
    isStopped = false;

    Serial.println("CMD: " + cmd);

    if (cmd == "F")           moveForward();
    else if (cmd == "B")      moveBackward();
    else if (cmd == "L")      turnLeft();
    else if (cmd == "R")      turnRight();
    else if (cmd == "S")      stopMotors();
    else if (cmd == "E")      moveForwardRight();
    else if (cmd == "Q")      moveForwardLeft();
    else if (cmd == "C")      moveBackwardRight();
    else if (cmd == "Z")      moveBackwardLeft();
    else if (cmd.startsWith("V") && cmd.length() == 2) {
        char level = cmd.charAt(1);
        if (level >= '0' && level <= '9')
            currentSpeed = speedTable[level - '0'];
        else if (level == 'A')
            currentSpeed = speedTable[10];
    }
    // en: F1, F2, F3 are general purpose digital outputs (HIGH=3.3V, LOW=0V). Assign functions as needed for your design.
    // tr: F1, F2, F3 genel amaçlı dijital çıkışlardır (HIGH=3.3V, LOW=0V). Tasarımınıza uygun işlev atayabilirsiniz.
    // es: F1, F2, F3 son salidas digitales de propósito general (HIGH=3.3V, LOW=0V). Asigne funciones según su diseño.
    // pt-PT: F1, F2, F3 são saídas digitais de uso geral (HIGH=3.3V, LOW=0V). Atribua funções conforme o seu design.
    // pt-BR: F1, F2, F3 são saídas digitais de uso geral (HIGH=3.3V, LOW=0V). Atribua funções conforme o seu design.
    else if (cmd == "H") { f1State = true;  digitalWrite(F1_PIN, HIGH); }
    else if (cmd == "h") { f1State = false; digitalWrite(F1_PIN, LOW);  }
    else if (cmd == "K") { f2State = true;  digitalWrite(F2_PIN, HIGH); }
    else if (cmd == "k") { f2State = false; digitalWrite(F2_PIN, LOW);  }
    else if (cmd == "T") { f3State = true;  digitalWrite(F3_PIN, HIGH); }
    else if (cmd == "t") { f3State = false; digitalWrite(F3_PIN, LOW);  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            // en: Stop the car immediately when WebSocket connection is lost.
            // tr: WebSocket bağlantısı koptuğunda aracı hemen durdur.
            // es: Detener el coche inmediatamente cuando se pierde la conexión WebSocket.
            // pt-PT: Parar o carro imediatamente quando a ligação WebSocket é perdida.
            // pt-BR: Parar o carro imediatamente quando a conexão WebSocket é perdida.
            stopMotors();
            break;

        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %s\n", num, ip.toString().c_str());
            webSocket.sendTXT(num, "ESP32 Connected!");
            break;
        }

        case WStype_TEXT: {
            String cmd = String((char*)payload);
            cmd.trim();
            if (cmd == "ping") {
                webSocket.sendTXT(num, "pong");
                lastCommandTime = millis();
                return;
            }
            handleCommand(cmd);
            break;
        }

        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(IN1_RIGHT, OUTPUT);
    pinMode(IN2_RIGHT, OUTPUT);
    pinMode(IN3_LEFT, OUTPUT);
    pinMode(IN4_LEFT, OUTPUT);
    pinMode(F1_PIN, OUTPUT);
    pinMode(F2_PIN, OUTPUT);
    pinMode(F3_PIN, OUTPUT);

    // en: Configure PWM channels for ESP32
    // tr: ESP32 için PWM kanallarını yapılandır
    // es: Configurar canales PWM para ESP32
    // pt-PT: Configurar canais PWM para ESP32
    // pt-BR: Configurar canais PWM para ESP32
    ledcAttachChannel(ENA_RIGHT, PWM_FREQ, PWM_RESOLUTION, ENA_RIGHT_CHANNEL);
    ledcAttachChannel(ENB_LEFT, PWM_FREQ, PWM_RESOLUTION, ENB_LEFT_CHANNEL);

    stopMotors();

    // en: lastCommandTime is initialized here to prevent the timeout from triggering immediately at startup.
    // tr: lastCommandTime burada başlatılır, böylece başlangıçta zaman aşımı tetiklenmez.
    // es: lastCommandTime se inicializa aquí para evitar que el timeout se active inmediatamente al inicio.
    // pt-PT: lastCommandTime é inicializado aqui para evitar que o timeout seja ativado imediatamente no arranque.
    // pt-BR: lastCommandTime é inicializado aqui para evitar que o timeout seja ativado imediatamente na inicialização.
    lastCommandTime = millis();

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("WebSocket server started on port 81");
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        // en: Stop the car and attempt to reconnect when WiFi is lost.
        // tr: WiFi bağlantısı koptuğunda aracı durdur ve yeniden bağlanmayı dene.
        // es: Detener el coche e intentar reconectar cuando se pierde el WiFi.
        // pt-PT: Parar o carro e tentar reconectar quando o WiFi é perdido.
        // pt-BR: Parar o carro e tentar reconectar quando o WiFi é perdido.
        stopMotors();
        Serial.println("WiFi lost, reconnecting...");
        WiFi.reconnect();
        delay(1000);
        return;
    }

    webSocket.loop();

    // en: If no command is received for 500ms, stop the car automatically.
    // tr: 500ms boyunca komut gelmezse aracı otomatik olarak durdur.
    // es: Si no se recibe ningún comando en 500ms, detener el coche automáticamente.
    // pt-PT: Se não for recebido nenhum comando em 500ms, parar o carro automaticamente.
    // pt-BR: Se nenhum comando for recebido em 500ms, parar o carro automaticamente.
    if (millis() - lastCommandTime > TIMEOUT && !isStopped) {
        stopMotors();
    }
}