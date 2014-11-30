# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('ac3app', '0005_auto_20141030_1808'),
    ]

    operations = [
        migrations.AddField(
            model_name='userprofile',
            name='hasTempPassword',
            field=models.BooleanField(default=False),
            preserve_default=True,
        ),
    ]
