# Image Resizer App

This is a simple web application to resize image encoded in Base64. The web server is using [libasyik](https://github.com/okyfirmansyah/libasyik), a web services that runs on C++ and Boost libraries.

## Build Docker
```
// Build from Dockerfile
docker build -t image-resizer-app .
```

## Run application
```
// Run app using docker
docker run -it --rm -p8080:8080 image-resizer-app /bin/bash ./image_resizer_app
```