from django.db import models
from django.contrib.auth.models import User
from datetime import timezone, datetime
from django.contrib.sessions.models import Session
from django.conf import settings


class UserProfile(models.Model):
    user = models.ForeignKey(User, unique=True)
    user_pin = models.IntegerField(max_length=5, default=1234)
    phone_number = models.CharField(max_length=15, null=True, blank=True)
    hasTempPassword = models.BooleanField(default=False)

    def natural_key(self):
        return self.user.username

    #override the __str__() method to return name instead of id
    def __str__(self):
        return self.user.username

    def get_full_profile(self, username):
        user = User.objects.get_by_natural_key(username)
        return user


class SensorManager(models.Manager):
    def get_by_natural_key(self, sensor_name, description_text):
        return self.get(sensor_name=sensor_name, description_text=description_text)


class Sensor(models.Model):
    objects = SensorManager()
    sensor_name = models.CharField(max_length=75)
    description_text = models.TextField(null=True, blank=True)

    def natural_key(self):
        return self.sensor_name

    def __str__(self):
        return self.sensor_name


class EventTypeManager(models.Manager):
    def get_by_natural_key(self, eventType_name, description_text):
        return self.get(eventType_name=eventType_name, description_text=description_text)


class EventType(models.Model):
    objects = EventTypeManager()
    eventType_name = models.CharField(max_length=75)
    description_text = models.TextField(null=True, blank=True)

    def natural_key(self):
        return self.eventType_name

    def __str__(self):
        return self.eventType_name


class Event(models.Model):
    user_id = models.ForeignKey(UserProfile, null=True, blank=True)
    date_created = models.DateField()
    time_created = models.TimeField(default='11:59:59')
    sensor_triggered = models.ForeignKey(Sensor, null=True, blank=True)
    event_image = models.ImageField(null=True, blank=True)
    event_type = models.ForeignKey(EventType)

    def was_created_recently(self):
        return self.date_created >= timezone.now() - datetime.timedelta(days=1)
    was_created_recently.admin_order_field = 'date_created'
    was_created_recently.boolean = True
    was_created_recently.short_description = 'Created recently?'


class UserSession(models.Model):
    user = models.ForeignKey(settings.AUTH_USER_MODEL)
    session = models.ForeignKey(Session)
