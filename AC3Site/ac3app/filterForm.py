from django import forms
from django.contrib.auth.models import User
from ac3app.models import Sensor, EventType
'''
Currently the way that FilterForm is defined the fields for the form
will be initialized on a server start. So any changes to our choice
fields will not be displayed (ie if a user was added to the DB that
user will not show up until the server restarts). This can be fixed
by calling the def __init__ method of the parent class(Form).

class FilterForm(forms.Form):
    def __init__(self, *args, **kwargs):
        super(FilterForm, self).__init__(*args, **kwargs)
        self.fields['user_field'] = forms.ChoiceField(choices=get_choices())

This will allow the form to be initialized each time the form is loaded.
Depending on how often it is called it may cause performance issues.
'''


def get_sensor_choices():
    sensors = Sensor.objects.all()
    senPairs = list()
    senPairs.append((0, ""))
    for sensor in sensors:
        senPairs.append((sensor.id, sensor.sensor_name))
    return senPairs


def get_event_choices():
    eventTypes = EventType.objects.all()
    eventPairs = list()
    eventPairs.append((0, ""))
    for eventType in eventTypes:
        eventPairs.append((eventType.id, eventType.eventType_name))
    return eventPairs


def get_user_choices():
    users = User.objects.all()
    userPairs = list()
    userPairs.append((0, ""))
    for user in users:
        if user.first_name != "":
            userPairs.append((user.id, user.first_name + " " + user.last_name))
    return userPairs


class FilterForm(forms.Form):
    user_choices = forms.ChoiceField(choices=get_user_choices())
    date1 = forms.DateField(widget=forms.TextInput(
        attrs={'id': 'datepicker1'}
    ))
    date2 = forms.DateField(widget=forms.TextInput(
        attrs={'id': 'datepicker2'}
    ))
    event_choices = forms.ChoiceField(choices=get_event_choices())
    sensor_choices = forms.ChoiceField(choices=get_sensor_choices())