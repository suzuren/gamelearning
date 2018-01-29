#!/bin/sh
ps aux | grep proxy_epoll

cat proxy_epoll.pid | xargs kill -10

ps aux | grep proxy_epoll
