FROM node:10.15.3

ENV  HOME="/app"
WORKDIR ${HOME}

RUN apt-get update && \
    apt-get install -y libpng-dev libgl1-mesa-dev gosu && \
    npm install -g yarn gulp

ADD package.json ${HOME}/package.json
ADD yarn.lock ${HOME}/yarn.lock

RUN yarn install --production=false

RUN mkdir ${HOME}/html-path
ADD ./html-path/index.js ${HOME}/html-path
ADD gulpfile.js ${HOME}/
ADD rev-manifest.json ${HOME}/

VOLUME /data
VOLUME /out

#COPY entrypoint.sh /usr/local/bin/entrypoint.sh
#RUN chmod +x /usr/local/bin/entrypoint.sh
#ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

