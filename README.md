# CDNify docker

## build & run 

```bash
$ docker build -t gulp_cdnify .
$ docker run gulp_cdnify gulp test
```

## Test

```bash
$ LOCAL_UID=$(id -u $USER) LOCAL_GID=$(id -g $USER) docker-compose run rev
$ LOCAL_UID=$(id -u $USER) LOCAL_GID=$(id -g $USER) docker-compose run absolute
$ LOCAL_UID=$(id -u $USER) LOCAL_GID=$(id -g $USER) docker-compose run fingerprint
$ LOCAL_UID=$(id -u $USER) LOCAL_GID=$(id -g $USER) docker-compose run cdnify
```