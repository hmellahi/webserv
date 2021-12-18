export REDIRECT_STATUS="true"
export REQUEST_METHOD="GET"
# export GATEWAY_INTERFACE="CGI/1.1"
# export CONTENT_LENGTH="0"
export SCRIPT_FILENAME="tests/testget.php"
export QUERY_STRING="subject=PHP&web=W3schools.com"
./cgi-bin/php-cgi 
# tests/testget.php