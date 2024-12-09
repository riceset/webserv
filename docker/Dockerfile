# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Dockerfile                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/09 17:09:01 by rmatsuba          #+#    #+#              #
#    Updated: 2024/12/09 17:59:15 by rmatsuba         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# install base image of ubuntu
FROM ubuntu:20.04

# update and install necessary tools
RUN apt-get update && apt-get install -y \
	build-essential \
	g++ \
	make \
	&& apt-get clean

# set working directory in container
WORKDIR /usr/src/app


