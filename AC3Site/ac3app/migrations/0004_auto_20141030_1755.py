# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('ac3app', '0003_usersession'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='event',
            name='picture_link',
        ),
        migrations.AddField(
            model_name='event',
            name='event_image',
            field=models.ImageField(null=True, blank=True, upload_to='media'),
            preserve_default=True,
        ),
    ]
