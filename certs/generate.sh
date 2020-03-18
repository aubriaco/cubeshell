#!/bin/sh
openssl genrsa -des3 -out cakey.pem 2048
openssl req -new -key cakey.pem -x509 -days 1000 -out cacert.pem
openssl genrsa -des3 -out node.key.pem 2048
openssl req -new -key node.key.pem -out node.csr
openssl x509 -req -days 365 -in node.csr -CA cacert.pem -CAkey cakey.pem -CAcreateserial -out node.pem
cat node.key.pem >> node.pem
