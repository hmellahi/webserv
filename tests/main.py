import requests

ID_USERNAME = 'signup-user-name'
ID_PASSWORD = 'signup-user-password'
USERNAME = 'username'
PASSWORD = 'yourpassword'
SIGNUP_URL = 'http://codepad.org/login'

def submit_form():
    """Submit a form"""
    payload = {ID_USERNAME : USERNAME, ID_PASSWORD : PASSWORD,}

    resp = requests.get(SIGNUP_URL)
    print "Response to GET request: %s" %resp.content

    resp = requests.post(SIGNUP_URL, payload)
    print "Headers from a POST request response: %s" %resp.headers
#print "HTML Response: %s" %resp.read()

if __name__ == '__main__':
    submit_form()
