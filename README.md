# Image Resizer App

This is a simple web application to resize image encoded in Base64. The web server is using [libasyik](https://github.com/okyfirmansyah/libasyik), a web services that runs on C++ and Boost libraries.

## Build Docker
```
// Build from Dockerfile
docker build -t image-resizer-app .

// [Optional] Run tests
docker run image-resizer-app /bin/bash -c "sh /workspace/image-resizer-app/tests/run_tests.sh"
```

## Run application
```
// Run app using docker
docker run -it --rm -p8080:8080 image-resizer-app ./build/image_resizer_app
```

## Examples
```
import base64
import json

import cv2
import numpy as np
import requests

image = np.random.randint(0, 256, (1080, 1920, 3), dtype=np.uint8)
image_string = base64.b64encode(cv2.imencode('.png', image)[1]).decode('utf-8')

payload = {
    "input_jpeg": image_string,
    "desired_width": 640,
    "desired_height": 480
}

response = requests.post("http://127.0.0.1:8080/resize_image", headers={"Content-Type":"application/json"}, data=json.dumps(payload))
response_dict = json.loads(response.text)

decoded_jpg_str = base64.b64decode(response_dict['output_jpeg'])
image_decoded = cv2.imdecode(np.fromstring(decoded_jpg_str, dtype=np.uint8), cv2.IMREAD_UNCHANGED)
assert(image_decoded.shape[:2] == (480, 640))

```