from django.conf.urls import patterns, include, url
from django.contrib import admin
from django.http import HttpResponseRedirect

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'AC3site.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),
    url(r'^admin/', include(admin.site.urls)),
    url(r'^ac3app/', include('ac3app.urls')),
    url(r'^$', lambda r : HttpResponseRedirect('ac3app/')),
)
