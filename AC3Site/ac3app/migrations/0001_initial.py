# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='Event',
            fields=[
                ('id', models.AutoField(serialize=False, auto_created=True, primary_key=True, verbose_name='ID')),
                ('date_created', models.DateField()),
                ('time_created', models.TimeField(default='11:59:59')),
                ('picture_link', models.FilePathField(null=True, blank=True)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='EventType',
            fields=[
                ('id', models.AutoField(serialize=False, auto_created=True, primary_key=True, verbose_name='ID')),
                ('eventType_name', models.CharField(max_length=75)),
                ('description_text', models.TextField(null=True, blank=True)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='Sensor',
            fields=[
                ('id', models.AutoField(serialize=False, auto_created=True, primary_key=True, verbose_name='ID')),
                ('sensor_name', models.CharField(max_length=75)),
                ('description_text', models.TextField(null=True, blank=True)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='UserProfile',
            fields=[
                ('id', models.AutoField(serialize=False, auto_created=True, primary_key=True, verbose_name='ID')),
                ('user_pin', models.IntegerField(default=1234, max_length=5)),
                ('user', models.ForeignKey(to=settings.AUTH_USER_MODEL, unique=True)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.AddField(
            model_name='event',
            name='event_type',
            field=models.ForeignKey(to='ac3app.EventType'),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='event',
            name='sensor_triggered',
            field=models.ForeignKey(to='ac3app.Sensor', blank=True, null=True),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='event',
            name='user_id',
            field=models.ForeignKey(to='ac3app.UserProfile', blank=True, null=True),
            preserve_default=True,
        ),
    ]
