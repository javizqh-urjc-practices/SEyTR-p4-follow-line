# Estructura de la practica

## Tareas

- Tarea: Motores; Prioridad: ; Encargado: Luis;
- Tarea: Ultrasonidos; Prioridad: ; Encargado: Luis;
- Tarea: Infrarrojos; Prioridad: ; Encargado: Javier;
- Tarea: Idle (Comunicación y debug led); Prioridad: 0; Encargado: Javier;

## Paso de información entre tareas

- De Infrarrojos a motores: poner un flag a LEFT, RIGHT, CENTER
- De Ultrasonidos a motores: poner un flag a STOP, que tiene mayor prioridad que los anteriores
- De Infrarrojos y Ultrasonidos a comunicación: añadir mensaje a una cola FIFO para ser enviados.
- De Idle a comunicación: si no hay mensaje mandar ping cada 4 segundos