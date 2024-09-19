# inicia contenedor denominado emqx
# Puerto 1883: MQTT sobre TCP
# Puerto 18083: dashboard
# Dashboard: 
# usuario: admin
# clave: public (luego pide cambiar)
printf "Iniciando contenedor....\n"
docker run -d --name emqx -p 1883:1883 -p 18083:18083 emqx:5.0.24
printf "Contenedor iniciado!\n"