from django.contrib import admin
from ac3app.models import Sensor, Event, UserProfile, EventType
# Register your models here.


class EventAdmin(admin.ModelAdmin):
    fieldsets = [
        (None,          {'fields': ['event_type', 'date_created']}),
        ('UserInfo',    {'fields': ['user_id']}),
        ('SensorInfo',  {'fields': ['sensor_triggered', 'event_image'], 'classes': ['collapse']}),
    ]
    list_display = ('id', 'event_type', 'date_created', 'user_id', 'sensor_triggered')
    list_filter = ['date_created', 'event_type']


class SensorAdmin(admin.ModelAdmin):
    fields = ['sensor_name', 'description_text']
    list_display = ('sensor_name', 'description_text')


class EventTypeAdmin(admin.ModelAdmin):
    fields = ['eventType_name', 'description_text']
    list_display = ('eventType_name', 'description_text')

admin.site.register(Event, EventAdmin)
admin.site.register(Sensor, SensorAdmin)
admin.site.register(EventType, EventTypeAdmin)
admin.site.register(UserProfile)