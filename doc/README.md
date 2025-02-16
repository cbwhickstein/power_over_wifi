# POW Documentation

This document concludes all the information to replicate this product.

## Layout

### General
```mermaid
graph LR;
    user[User] --> |http| mcu[**ESP32-FreeRTOS** <br> -Website <br> -connection to PC];
    mcu --> |GPIO| PC;
    user --> |ssh| PC;
```

### MCU

```mermaid
stateDiagram
    direction LR
    [*] --> init
    init --> Webserver
    Webserver --> Idle
    Idle --> Webserver
    init --> GPIO
    Webserver --> GPIO
    GPIO --> Webserver
```

