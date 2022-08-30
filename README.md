# YAKU
Este codigo es parte de un proyecto llamado **Yaku** de los estudiantes de la ***ET36 D.E. 15***,
el cual tiene como objetivo crear una huerta inteligente para el uso de ***Escuela de Discapacitados Motores Nº 1 D.E 15.***

## ToDo's
- [x] Guia rainmaker
  - [x] Separar los pasos, con título y numerados.
  - [x] La creación del nodo no la tienen que hacer los demás, separarlo de la guía
  - [x] Si dejar la parte de cómo agregar un device al nodo.
- [ ] Como funciona el codigo
- [x] Añadir links/referencias

## Como funciona el codigo
__Completar__

## GUIA RAINMAKER (YAKU)

Paso a paso de como crear un dispositivo, añadirlo a un nodo y como subir los datos a la app de RAINMAKER.

### Paso 1: Crear sensor dentro del código.
Escribir una variable del tipo del sensor* (de ahora en más llamaremos a estos **DISPOSITIVOS**) seguido del nombre que le quieras atribuir a este dentro del código además del nombre dentro de la app. Esto se definiría de la siguiente manera:

```bash
  static TemperatureSensor humidity("Humidity"); 
```

En este caso estamos creando un dispositivo de temperatura (Ya que Rainmaker no tiene de humedad), con el nombre **humidity** tanto dentro del codigo como en la app.

> **Rainmaker nos proporciona 4 tipos de dispositivos, los cuales son TemperatureSensor, Switch, Fan y LightBulb.*

### Paso 2: Agregar nuestro dispositivo al nodo
Utilizando el metodo addDevice() y utilizando como parametro el dispositivo creado anteriormente (Este paso se tiene que realizar con cada dispositivo que creemos).

```bash
  my_node.addDevice(humidity);
```

### Paso 3: Leer los datos de los dispositivos y subirlos a la app de RainMaker.
Para realizar esto lo que haremos sera leer el pin al cual tenemos conectado el dispositivo y asignar este valor una variable. Luego utilizando la variable de nuestro dispositivo usaremos el metodo updateAndReportParam() el cual tiene como parametros el tipo de sensor que es (En este caso es Temperature) y el valor que le queramos subir.

```bash
  float sensor_value = analogRead(gpio_humidity);
  humidity.updateAndReportParam("Temperature", sensor_value);
```

## Referencias utilizadas en el proyecto
- https://www.youtube.com/watch?v=651EoGQHWck


### Recordar
- constrain()
