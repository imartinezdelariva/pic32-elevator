# Three-Floor Elevator Control System with PIC32

An embedded control system for a physical three-floor elevator prototype, developed in C using a PIC32 microcontroller.

The system processes floor requests, controls the direction and speed of the motor, detects the elevator's position through magnetic sensors and implements an emergency return procedure. Commands can be received through the physical buttons or over UART.

## Demo

[Watch the elevator prototype in action](https://www.youtube.com/shorts/Xl_p0GWCGI4)

## Main features

- Three selectable floors: 0, 1 and 2
- Magnetic sensor at each floor for position detection
- Queue-like management of pending floor requests
- First-call priority when requests require opposite directions
- Bidirectional DC motor control using PWM
- Physical floor buttons and UART commands
- Emergency stop with audible alarm
- Automatic return to floor 0 after an emergency
- Automatic initial calibration to floor 0 after reset

## How it works

The pending requests are stored in a three-position array:

```c
int PISOS[3] = {0, 0, 0};
```

Each position represents one floor. When a floor is requested, its corresponding value is set to `1`. When the elevator reaches that floor and its magnetic sensor is activated, the request is cleared by setting the value back to `0`.

For example:

```text
[0, 0, 0]  No pending requests
[0, 0, 1]  Floor 2 requested
[1, 0, 1]  Floors 0 and 2 requested
```

The controller continuously reads the buttons, UART input and magnetic sensors. It then chooses the motor direction according to the current floor and pending requests.

## Request priority

When the elevator is on floor 1 and receives requests for both floor 0 and floor 2, the system remembers which of the two calls arrived first. The elevator serves that request before reversing direction for the other one.

If floor 1 is requested while the elevator is travelling between the lower and upper floors, its magnetic sensor allows the controller to stop and clear the request when the cabin reaches that floor.

## Floor detection

Each floor has a magnetic detector connected to the microcontroller. Edge detection is used to identify the moment when a sensor becomes active.

When the requested floor is detected, the controller:

1. Stops the motor.
2. Updates the current floor.
3. Clears the corresponding pending request.
4. Waits before processing the next movement.

## Motor control

The motor is controlled through the Output Compare module using PWM:

- One digital output selects the direction.
- The PWM output controls motor activation.
- `girar_izquierda()` moves the elevator upwards.
- `girar_derecha()` moves the elevator downwards.
- `parar_motor()` sets the PWM duty cycle to zero.

The firmware represents the elevator with three operating states:

```c
typedef enum {
    PARADO_ESTADO,
    SUBIENDO,
    BAJANDO
} EstadoAscensor;
```

## Emergency procedure

The emergency command immediately:

1. Stops the motor.
2. Activates the buzzer.
3. Clears all pending floor requests.
4. Waits five seconds using Timer 4.
5. Moves the elevator safely down to floor 0.
6. Stops the motor and deactivates the buzzer.

This ensures that the prototype returns to a known position after an emergency.

## UART control

The UART interface operates at 9,600 baud and uses interrupt-driven circular buffers for transmission and reception.

The accepted commands are:

| Command | Action |
| --- | --- |
| `0` | Request floor 0 |
| `1` | Request floor 1 |
| `2` | Request floor 2 |
| `e` | Activate the emergency procedure |

## Software modules

| File | Responsibility |
| --- | --- |
| `Main.c` | Main control loop, request handling, floor sensors and emergency logic |
| `motor.c/.h` | Motor direction and PWM control |
| `bocina.c/.h` | Buzzer initialisation and control |
| `UART.c/.h` | Interrupt-driven serial communication and circular buffers |
| `Pic32Ini.c/.h` | PIC32 clock initialisation |
| `Makefile` | Project build configuration |

## Technologies and concepts

- Embedded C
- PIC32 microcontroller
- MPLAB X and XC32
- GPIO
- PWM and Output Compare
- Hardware timers and interrupts
- UART communication
- Circular buffers
- Magnetic position sensors
- Edge detection
- State-based control logic

## Project background

This project was developed for a Microprocessors Laboratory course. It combines low-level peripheral configuration with the control logic required to operate a physical elevator prototype.

## My Contribution

This project was developed collaboratively by a two-person team, with implementation work shared broadly across the system.

## Author

**Miguel Naranjo Ruiz-Socolado**
**Iñigo Martínez de la Riva Muinelo**  
Telecommunication Technologies Engineering  
Universidad Pontificia Comillas - ICAI
