# 📟 Cómo Programar ESP32-CAM con Arduino Uno

Si no tienes un programador FTDI, puedes usar tu Arduino Uno como puente USB-Serial para programar la ESP32-CAM.

---

## 🎯 Dos Métodos Disponibles

### Método 1: Arduino Uno como Puente USB-TTL (Más Simple)
### Método 2: Quitar el Chip ATmega328P (Alternativa)

Recomiendo el **Método 1** porque no requiere desmontar nada.

---

## ⚡ MÉTODO 1: Arduino Uno como Puente USB-TTL (RECOMENDADO)

Este método usa el Arduino Uno solo como conversor USB-Serial, sin usar su microcontrolador.

### Paso 1: Preparar Arduino Uno

**Opción A - Cortocircuitar RESET a GND (Más simple)**

1. Conectar un **cable/jumper entre RESET y GND** del Arduino Uno
2. Esto desactiva el ATmega328P
3. El Arduino Uno ahora funciona solo como conversor USB-TTL

```
Arduino Uno:
┌─────────────┐
│   RESET  ●──┼───┐
│          │  │   │ Cable jumper
│   GND    ●──┼───┘
│             │
└─────────────┘
```

**Opción B - Capacitor de 10µF entre RESET y GND**

1. Conectar capacitor de 10µF
2. Polo negativo (-) a GND
3. Polo positivo (+) a RESET
4. Esto mantiene el ATmega en reset

---

### Paso 2: Conexiones Arduino Uno → ESP32-CAM

```
Arduino Uno          ESP32-CAM
═══════════════════════════════════
5V           →       5V
GND          →       GND
TX (pin 1)   →       U0R (RX)
RX (pin 0)   →       U0T (TX)
GND          →       GPIO 0 (para programar)
```

**Diagrama Visual:**

```
┌─────────────────┐                    ┌──────────────┐
│  Arduino Uno    │                    │ ESP32-CAM    │
│                 │                    │              │
│  RESET ●────●   │                    │              │
│         GND     │                    │              │
│                 │                    │              │
│  5V      ●──────┼────────────────────┼──→ 5V        │
│  GND     ●──────┼────────────────────┼──→ GND       │
│  TX (1)  ●──────┼────────────────────┼──→ U0R (RX)  │
│  RX (0)  ●──────┼────────────────────┼──→ U0T (TX)  │
│  GND     ●──────┼───┐                │              │
│                 │   │ Para programar │              │
│                 │   └────────────────┼──→ GPIO 0    │
└─────────────────┘                    └──────────────┘
     │                                        
     │ Cable USB al PC
     ▼
```

---

### Paso 3: Modo Programación

Para entrar en modo programación de la ESP32-CAM:

1. **Conectar GPIO 0 a GND** con un cable jumper
2. **Presionar RESET** en la ESP32-CAM (o desconectar/reconectar alimentación)
3. Ahora está en modo bootloader
4. **Dejar GPIO 0 conectado a GND** durante toda la carga

---

### Paso 4: Configurar Arduino IDE

1. Abrir Arduino IDE
2. Ir a `Herramientas` → `Placa` → `ESP32 Arduino` → `AI Thinker ESP32-CAM`
3. Ir a `Herramientas` → `Puerto` → Seleccionar el puerto del Arduino Uno
   - **Windows**: Generalmente COM3, COM4, etc.
   - **Linux/Mac**: /dev/ttyUSB0, /dev/ttyACM0
4. `Herramientas` → `Velocidad de Subida` → **115200**

---

### Paso 5: Cargar el Código

1. Abrir `ESP32_CAM_Robot_Control.ino`
2. Modificar SSID y contraseña WiFi (líneas 30-31)
3. Click en **✓ Verificar** (esperar a que compile)
4. Click en **→ Subir**
5. **Esperar** (puede tardar 1-3 minutos)
6. Mensaje: "Done uploading" o "Carga completada"

---

### Paso 6: Modo Normal (Ejecutar)

Una vez cargado:

1. **DESCONECTAR el cable entre GPIO 0 y GND**
2. **Presionar RESET** en la ESP32-CAM
3. Ahora ejecutará el programa
4. Abrir Monitor Serie (115200 baudios) para ver la IP

---

## 🔧 MÉTODO 2: Quitar Chip ATmega328P

Este método requiere extraer el chip del Arduino Uno.

### ⚠️ Advertencias:

- Solo para Arduino Uno con chip **extraíble** (DIP)
- NO para Arduino Uno SMD (chip soldado)
- Riesgo de dañar pines del chip al extraer

### Pasos:

1. **Desconectar** Arduino Uno del USB
2. Con cuidado, usar destornillador plano para **extraer el ATmega328P**
3. Hacer conexiones normales (sin jumper RESET-GND):

```
Arduino Uno (sin chip)    ESP32-CAM
════════════════════════════════════
5V                   →    5V
GND                  →    GND
TX (pin 1)           →    U0R (RX)
RX (pin 0)           →    U0T (TX)
```

4. Para programar: GPIO 0 a GND
5. Cargar código normalmente
6. **Reinsertar el chip** cuando termines

---

## 🐛 Solución de Problemas

### Error "Failed to connect to ESP32"

**Solución 1**: Verificar que RESET esté cortocircuitado a GND en Arduino Uno

```bash
# Verificar:
1. Jumper RESET-GND bien conectado
2. GPIO 0 conectado a GND en ESP32-CAM
3. Presionar RESET de ESP32-CAM
4. Intentar cargar de nuevo
```

**Solución 2**: Reducir velocidad de carga

```
Herramientas → Velocidad de Subida → 9600
```

**Solución 3**: Secuencia correcta

```
1. Conectar GPIO 0 a GND (ESP32-CAM)
2. Presionar y MANTENER botón RESET (ESP32-CAM)
3. Click en "Subir" en Arduino IDE
4. Soltar RESET cuando empiece "Connecting..."
```

---

### No aparece puerto COM

**Windows**: Instalar drivers CH340
- Descargar: https://sparks.gogo.co.nz/ch340.html
- Instalar y reiniciar PC

**Linux**: Agregar permisos
```bash
sudo usermod -a -G dialout $USER
sudo chmod 666 /dev/ttyACM0  # o ttyUSB0
```

---

### "Brownout detector triggered" al cargar

**Causa**: Alimentación insuficiente del USB del PC

**Solución**:
1. Usar hub USB con alimentación externa
2. O alimentar ESP32-CAM desde fuente externa 5V/2A:
   ```
   Fuente 5V → ESP32-CAM (5V y GND)
   Arduino Uno → Solo TX, RX, GND a ESP32-CAM
   ```

---

### Carga muy lenta o se detiene al 99%

**Solución**:
```
1. Reducir velocidad: 115200 → 57600
2. Verificar calidad de cables (usar cortos)
3. Probar otro puerto USB del PC
```

---

## ✅ Verificación Post-Carga

### Paso 1: Monitor Serie

1. Desconectar GPIO 0 de GND
2. Presionar RESET
3. Abrir Monitor Serie (115200 baudios)
4. Deberías ver:

```
=================================
ESP32-CAM Robot Control System
=================================
WiFi conectado exitosamente
Dirección IP: 192.168.1.XXX
Sistema listo!
=================================
```

### Paso 2: Desconectar Arduino Uno

Una vez verificado que funciona:

1. Desconectar USB del Arduino Uno
2. Desconectar todos los cables
3. ESP32-CAM ya tiene el programa cargado permanentemente
4. Alimentar ESP32-CAM directamente con fuente 5V para usar

**El programa queda grabado en la ESP32-CAM**, no necesitas el Arduino Uno para ejecutarlo, solo lo usas para programar.

---

## 🔄 Para Cargar Código Nuevamente

Cada vez que quieras actualizar el programa:

1. Volver a conectar Arduino Uno como puente
2. RESET a GND en Arduino Uno
3. GPIO 0 a GND en ESP32-CAM
4. Cargar código
5. Desconectar GPIO 0
6. RESET en ESP32-CAM

---

## 💡 Consejos Adicionales

### Usar Cables de Calidad

```
✅ Cables cortos (menos de 30cm)
✅ Cables gruesos (calibre 22-24 AWG)
❌ Cables protoboard (mala conexión)
❌ Cables muy largos (caída de voltaje)
```

### Verificar Conexiones

Antes de cargar, verificar con multímetro:
- 5V entre 5V y GND ≈ 5.0V
- TX y RX no están en corto
- GPIO 0 está a 0V (GND) durante programación

### Puerto Serie Ocupado

Si dice "Puerto ya en uso":
```
1. Cerrar Monitor Serie
2. Cerrar otras instancias de Arduino IDE
3. Desconectar y reconectar USB
4. Intentar de nuevo
```

---

## 📊 Comparación de Métodos

| Característica | Arduino Uno como Puente | Programador FTDI |
|----------------|-------------------------|------------------|
| **Costo** | $0 (ya lo tienes) | $3-6 |
| **Velocidad** | Normal | Normal |
| **Confiabilidad** | Buena | Excelente |
| **Facilidad** | Media | Fácil |
| **Modificación** | Jumper RESET-GND | Ninguna |
| **Cables necesarios** | 5 | 4 |

---

## 🎯 Resumen Rápido

### Configuración:
```
1. Jumper RESET-GND en Arduino Uno
2. Conectar 5V, GND, TX, RX
3. GPIO 0 a GND en ESP32-CAM para programar
```

### Carga:
```
1. Arduino IDE → Placa: AI Thinker ESP32-CAM
2. Puerto: COM del Arduino Uno
3. Modificar WiFi en código
4. Subir
```

### Ejecución:
```
1. Desconectar GPIO 0 de GND
2. RESET en ESP32-CAM
3. Ver IP en Monitor Serie
4. Abrir en navegador
```

---

## 🔗 Recursos Adicionales

### Drivers CH340 (Windows)
- https://sparks.gogo.co.nz/ch340.html

### Video Tutorial (ESP32-CAM con Arduino Uno)
- Buscar en YouTube: "ESP32-CAM Arduino Uno programmer"

### Pinout Arduino Uno
```
┌──────────────┐
│  Arduino Uno │
├──────────────┤
│ RESET    ●   │
│ 3.3V     ●   │
│ 5V       ●   │ ← Usar este
│ GND      ●   │ ← Común
│ GND      ●   │ ← Disponible
│          .   │
│ RX (0)   ●   │ ← A TX ESP32
│ TX (1)   ●   │ ← A RX ESP32
└──────────────┘
```

---

## ❓ Preguntas Frecuentes

### ¿Puedo usar Arduino Mega en lugar de Uno?

**Sí**, mismo procedimiento:
- Jumper RESET-GND
- Usar 5V, GND, TX0, RX0

### ¿Puedo usar Arduino Nano?

**Sí**, exactamente igual:
- Jumper RESET-GND
- Pines D0 (RX) y D1 (TX)

### ¿El programa se borra al desconectar?

**No**, el programa queda grabado en la flash de la ESP32-CAM permanentemente. Solo se borra si cargas otro programa.

### ¿Necesito el Arduino Uno conectado para que funcione?

**No**, solo lo necesitas para programar. Una vez cargado el código:
1. Desconecta todo
2. Alimenta ESP32-CAM directamente con 5V
3. Funciona independientemente

### ¿Puedo programar múltiples ESP32-CAM?

**Sí**, el Arduino Uno puede programar tantas ESP32-CAM como quieras, una por vez.

---

## 🎬 Procedimiento Completo en 10 Pasos

1. ✅ Conectar jumper RESET-GND en Arduino Uno
2. ✅ Conectar 5V, GND, TX, RX entre Arduino y ESP32
3. ✅ Conectar GPIO 0 a GND en ESP32-CAM
4. ✅ Conectar Arduino Uno al PC por USB
5. ✅ Abrir Arduino IDE, seleccionar placa ESP32-CAM
6. ✅ Seleccionar puerto del Arduino Uno
7. ✅ Modificar WiFi en código
8. ✅ Cargar código (esperar 1-3 minutos)
9. ✅ Desconectar GPIO 0 de GND
10. ✅ Presionar RESET en ESP32-CAM → ¡Listo!

---

**¡Listo para programar tu ESP32-CAM con Arduino Uno!** 🚀

Si tienes problemas, revisa la sección de Solución de Problemas o consulta en los foros de ESP32.

---

**Última actualización**: Mayo 2026
