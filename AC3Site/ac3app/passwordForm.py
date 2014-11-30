from django import forms
from django.contrib.auth.models import User

def get_users():
    users = User.objects.all()
    userlist = list()
    userlist.append((0, ""))
    for user in users:
        if user.first_name != "":
            userlist.append((user.username, user.username))
    return userlist

class PasswordForm(forms.Form):
    chooseuser = forms.ChoiceField(required=True,choices=get_users(), initial="admin")
    nPassword = forms.CharField(widget=forms.PasswordInput)
    cPassword = forms.CharField(widget=forms.PasswordInput)


