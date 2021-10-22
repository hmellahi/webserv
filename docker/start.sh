docker build . --name mynginxenv
docker run -it -p 8000:80 mynginxenv bash