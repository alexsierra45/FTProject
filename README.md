# FTProject

## Autores

- [Alex Sierra Alcala](https://github.com/alexsierra45)
- [Juan Carlos Espinosa Delgado](https://github.com/Jky45)

## Introduccion

Este proyecto tiene como objetivo crear un servidor web que le de la posibilidad a los clientes de navegar por un directorio de carpetas pudiendo descargar archivos. Además el servidor puede manejar múltiples clientes de forma concurrente.
Al ejecutarlo se le puede especificar el puerto y el root del servidor, de lo contrario se configura el puerto 5000 y el root del sistema.

Un cliente puede realizar las siguientes acciones:
* Crear una lista de los archivos ubicados en el directorio actual, especificando el nombre, tamaño y la ultima fecha en la que fue modificado.
* Cambiar de directorio, tanto al directorio padre como a un directorio hijo.
* Descargar un archivo del servidor.

## Implementación

El flujo de los datos comienza en el método `main` que se encarga de definir el puerto y el root del servidor, en caso de que hayan sido introducidos valida que el puerto sea un entero positivo y que el directorio exista y tenga permisos de lectura, en caso de que no se hayan introducido se configura el puerto 5000 y el root del sistema.
Luego llama al método `loop` que se encarga de inicializar el servidor en el puerto especificado y de la creación de los procesos que manejan las peticiones a través del método `handle_client` de `operations`.

### Manejo de la Solicitud
Cuando se recibe una solicitud en el método handle_client se analiza que el método sea GET, además se revisa que la url no esté vacía, 
en caso de que no se cumpla alguna de estas condiciones se envía una respuesta con código de estado `400 Bad Request`. Si se cumple con las condiciones se analiza si el path es un archivo o un directorio, en caso de que sea un archivo se llama al método `download_file`, en caso de que sea un directorio se llama al método `navigate`. En caso de que el path no exista se envía una respuesta con código de estado `404 Not Found`.

En caso de que la descarga del archivo tenga éxito se envía una respuesta HTTP con código de estado `200 OK`, con headers `Content-Type`, `Content-Length` y `Content-Disposition` y el contenido es el archivo que se desea descargar.

En caso de que se tenga éxito al cambiar de directorio se envía el contenido del directorio, a través de una respuesta HTTP, que es construida en `render`, esta respuesta tiene código de estado `200 OK`,
un header `Content-Type` que es `html` y el contenido se arma dinámicamente tomando la plantilla que se encuentra en el archivo `index.html` e insertando la tabla con el contenido del directorio, se listan los archivos del directorio y se arman las columnas de la tabla con sus respectivos nombre, tamaño y fecha de modificación.
Se puede ordenar por cualquiera de los campos mencionados simplemente haciendo clic sobre el nombre del campo, esta ordenación se realiza. Además contamos con un botón para volver al directorio padre.

### Errores

El servidor maneja los siguientes errores:

- `400 Bad Request`: se envía cuando la solicitud del cliente no es válida, por ejemplo cuando el método no es `GET`.
- `403 Forbidden`: se envía cuando el archivo o directorio al que se desea acceder no tiene permisos de lectura.
- `404 Not Found`: se envía cuando el archivo o directorio al que se desea acceder no existe.
- `500 Internal Server Error`: se envía cuando ocurre un error interno en el servidor.

## Múltiples Clientes

El servidor puede manejar múltiples clientes de forma concurrente, para esto se crean procesos que se encargan de manejar a cada petición, esto ocurre en el método `loop` del `main`.

## Compilación y ejecución

Se requiere tener instalada la libreria libcurl. Si está utilizando Ubuntu o Debian, puede instalarla con el siguiente comando en la terminal:

```
sudo apt-get install libcurl4-openssl-dev
```

Para compilar y ejecutar el proyecto, sigue los siguientes pasos:

1. Clona el repositorio desde GitHub.
2. Navega a la carpeta del proyecto.
3. Ejecuta el comando `make` para compilar y ejecutar el proyecto con los valores predeterminados.
4. Para introducir nuevos parametros, ejecute el siguiente comando en la terminal con los valores deseados:

```
make port=<port> root=<root>
```