<?php header("Location: localhost:8002/myOtherPage.php");
/* Make sure that code below does not get executed when we redirect. */
header('WWW-Authenticate: Negotiate');
header('WWW-Authenticate: NTLM', false);
exit;
?>