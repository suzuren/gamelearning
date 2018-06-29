#!/bin/sh
ps aux | grep test_logger

cat logger.pid | xargs kill -10

ps aux | grep test_logger
