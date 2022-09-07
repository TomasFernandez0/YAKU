# YAKU
Este codigo es parte de un proyecto llamado **Yaku** de los estudiantes de la ***ET36 D.E. 15***,
el cual tiene como objetivo crear una huerta inteligente para el uso de ***Escuela de Discapacitados Motores Nº 1 D.E 15.***

## ToDo's
- [x] Guia rainmaker
  - [x] Separar los pasos, con título y numerados.
  - [x] La creación del nodo no la tienen que hacer los demás, separarlo de la guía
  - [x] Si dejar la parte de cómo agregar un device al nodo.
- [x] Como funciona el codigo
- [x] Añadir links/referencias

## Como funciona el codigo

### 1. Se definen valores por defecto (por ejemplo: valor maximo y minimo de un sensor) y se declaran los distintos pines en los que se conectan los dispositivos
```bash
const float max_value_humidity = 3500;
const float min_value_humidity = 0; 
```

### 2. Se crean los dispositivos a utilizar
```bash

static TemperatureSensor humidity("Humedad"); //Sensor de humedad 
static TemperatureSensor ldr("LDR"); //Sensor de luminosidad
static Switch my_switch("Relay", &relay); //relay

```

### 3. Se crea una funcion la cual sera capaz de obtener las credenciales y conectarlas al enrutador

### 4. Se crea un callback, el cual detecta al relay (de que dispositivo se estan obteniendo los datos, y mas especificamente que parametro del dispositivo se necesita), y luego detecta si este parametro se encuentra encendido o no, para así encender o apagar el relay.

### 5. En el setup se inicializa el puerto serie, los pines. Se crea el nodo y se agregan al nodo todos los dispositivos a utilizar

```bash
	Node my_node;
  	my_node = RMaker.initNode("ET.36");
	my_node.addDevice(humidity);
  	my_node.addDevice(ldr);
  	my_node.addDevice(my_switch);
```

### 6. Tambien en el setup, se declara la funcion callback para el switch, la cual es responsable de recibir los datos de las aplicaciones moviles, por lo tanto, cuando se envie cualquier dato de la aplicacion movil, se llamará al callback (explicado en el punto 4).

```bash
	 my_switch.addCb(write_callback);

```

### 7. Se inician los servicios de RainMaker.

```bash
	Serial.printf("\nStarting ESP-RainMaker\n");
  	RMaker.start();
```

### 8. En el loop chequea que todo se encuentre en condiciones y envia los datos obtenidos a traves de 3 funciones: 
- La primera se encarga de actualizar los valores de los dispositivos (sensores).
- La segunda:
	- Se encarga de mapear los datos que se obtienen de los sensores en valores mas intuitivos para el usuario (porcentajes de 1 a 100).
	- Muestra en el puerto serie los valores y porcentajes obtenidos de los sensores.
	- Envia los valores obtenidos a rainMaker.
- La tercera funcion es responsable de abrir la bomba de agua durante 5 segundos cada 12 horas dependiendo de las condiciones que reportan los dispositivos.

```bash

	if (Timer.isReady() && wifi_connected) { 
	    Serial.println("Sending Sensor's Data");
	    Update_Sensor();
	    Send_Sensor();
	    Regado();
	    Timer.reset(); 
  	};

```
#### Todos los datos reportados se enviaran de acuerdo a un contador (timer)

### 9. Por ultimo, se configura la opcion de presionar el boton de reseteo de credenciales. El cual si se mantiene presionado entre 3 y 10 segundos, reseteará WiFi. Y si se mantiene presionado mas de 10 segundos, se resetea de fabrica.

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
