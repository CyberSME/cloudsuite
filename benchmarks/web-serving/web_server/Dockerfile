FROM ubuntu:14.04
LABEL maintainer="Mark Sutherland <mark.sutherland@epfl.ch>"

USER root

RUN apt-get update && apt-get install -y \
	build-essential \
	git \
	nginx \
	php5 php5-gd \
	php5-mysql php5-curl \
	php5-fpm php5-memcache	

# Increase the open file limit
COPY files/limits.conf.append /tmp/
RUN cat /tmp/limits.conf.append >> /etc/security/limits.conf && rm -f /tmp/limits.conf.append

# Checkout the Elgg installation
COPY files/elgg_installation /usr/share/nginx/html/elgg

WORKDIR /usr/share/nginx/html
# Copy over the settings.php
COPY files/settings.php elgg/engine/settings.php

# Make the Elgg data directory
RUN mkdir /elgg_data
RUN chmod a+rw /elgg_data

# Copy over the Nginx Server configuration
COPY files/nginx_sites_avail.append /tmp/
RUN cat /tmp/nginx_sites_avail.append >> /etc/nginx/sites-available/default

RUN service nginx restart

RUN service php5-fpm restart

ADD bootstrap.sh /etc/bootstrap.sh
RUN chown root:root /etc/bootstrap.sh
RUN chmod 700 /etc/bootstrap.sh

EXPOSE 8080

CMD ["/etc/bootstrap.sh", "-d"]
