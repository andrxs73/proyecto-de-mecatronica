# Control inalámbrico de un LED con ESP32

**Asignatura:** Fundamentos de Mecatrónica
**Grupo:** G01 <!-- cambia por tu número de grupo -->
**Integrantes:** <!-- nombres -->

El ESP32 crea su propia red Wi-Fi (modo punto de acceso) y aloja una página web con botones para encender y apagar un LED desde el navegador de un celular, sin internet.

## Materiales

| Cantidad | Elemento | Función |
|---|---|---|
| 1 | ESP32 DevKit / ESP32-WROOM-32 | Controlador y servidor web |
| 1 | LED rojo | Indicador visual |
| 1 | Resistencia de 220 Ω | Limitar la corriente |
| 1 | Protoboard | Montaje sin soldadura |
| 2-3 | Cables Dupont | Conexiones |
| 1 | Cable USB de datos | Programación y energía |

## Diagrama de conexiones

```text
GPIO18 del ESP32 --- [ 220 ohmios ] --- |>| --- GND del ESP32
                                          LED
                                   anodo -> catodo
```

Corriente estimada: (3,3 V − 2,0 V) / 220 Ω ≈ 5,9 mA.

![Foto del montaje](fotos/montaje.jpg)

## Cómo usarlo

1. Abre `Control_LED_ESP32_WiFi_Retos.ino` en Arduino IDE 2.x con el paquete *esp32 by Espressif Systems* instalado.
2. Selecciona **ESP32 Dev Module** y el puerto, y pulsa **Subir**.
3. Abre el Monitor serie a 115200 baudios y verifica que aparezca la IP `192.168.4.1`.
4. En el celular conéctate a la red `ESP32_LED_G01` (clave `Mecatronica2026`).
5. Abre `http://192.168.4.1/` (con `http://`, no `https://`).

![Captura de la red Wi-Fi](fotos/red_wifi.png)
![Captura de la interfaz](fotos/interfaz.png)

## Retos implementados

| Reto | Descripción | Implementación |
|---|---|---|
| 1 | SSID con grupo, título propio, GPIO18 | `NOMBRE_RED`, título HTML y `PIN_LED = 18`, con el circuito movido a GPIO18 |
| 2 | Un solo botón para alternar | Ruta `POST /alternar` y función `alternarLED()` |
| 3 | Autoapagado a los 10 s sin bloquear | `millis()` y `marcaEncendido` comprobados en `loop()` |

### Por qué no bastaba cambiar solo el software (Reto 1)

`PIN_LED` indica qué pin controla el programa, pero el cable lo conecta físicamente a un pin concreto. Si el cable sigue en GPIO23 y el código controla GPIO18, la señal sale por un pin que no está conectado al LED.

### Reto 3: conceptos

- **`millis()`:** devuelve los milisegundos transcurridos desde el arranque del ESP32.
- **Marca temporal:** valor de `millis()` guardado en un instante; al restarlo del valor actual se obtiene el tiempo transcurrido.
- **Bloqueante vs. no bloqueante:** `delay(10000)` detiene el programa y el servidor deja de responder. Con `millis()`, `loop()` sigue ejecutándose, atiende peticiones y solo comprueba el tiempo en cada vuelta.

## Pruebas de aceptación

| Prueba | Resultado esperado | Resultado obtenido |
|---|---|---|
| Reto 2: 3 pulsaciones desde apagado | Encendido, apagado, encendido | <!-- completar --> |
| Reto 3a: encender y esperar | Se apaga solo a ~10 s | <!-- completar --> |
| Reto 3b: Apagar antes de 10 s | Se apaga de inmediato | <!-- completar --> |
| Reto 3c: Encender de nuevo antes de 10 s | El plazo de 10 s se reinicia | <!-- completar --> |

## Preguntas de la guía

1. **¿Por qué funciona sin internet?** El ESP32 crea su propia red Wi-Fi y aloja el servidor. El celular solo necesita llegar a `192.168.4.1`, un dispositivo de esa red local.
2. **¿Qué hace `handleClient()`?** Revisa si hay una solicitud HTTP pendiente y ejecuta la función asociada a su ruta. Sin ella, el servidor está configurado pero no atiende a nadie.
3. **¿Qué riesgo hay al conectar el LED sin resistencia?** La corriente solo la limitaría el LED, que ofrece muy poca resistencia al conducir. Puede circular más corriente de la que soportan el LED y el GPIO, y dañarse uno de los dos o ambos.
4. **¿Qué diferencia hay entre el estado físico y `ledEncendido`?** La variable es lo que el programa cree que ocurre; el estado físico es si realmente circula corriente por el LED. Pueden diferir por un cable suelto, un LED invertido o un pin equivocado: la página diría ENCENDIDO sin que haya luz.

## Video de demostración

<!-- Enlace al video: encendido, apagado, alternar y autoapagado -->

## Seguridad y límites

Prototipo para un LED de bajo consumo. HTTP local sin cifrado; el único control de acceso es la contraseña de la red. No conectar motores, relés ni tiras de LED directamente a un GPIO.
