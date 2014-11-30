from django.shortcuts import render
from django.http import HttpResponse, HttpResponseRedirect
from ac3app.models import Event, UserProfile, UserSession
from django.contrib.auth import logout, authenticate, login
from django.core import serializers
from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.contrib import messages
from ac3app.models import Event
from ac3app import emails
from ac3app.passwordForm import PasswordForm
from ac3app.filterForm import FilterForm


def index(request):
    try:
        if request.COOKIES['logged'] == 'temp':
            return HttpResponseRedirect('/ac3app/mainview/')
    except KeyError:
        pass
    return render(request, 'ac3app/login.html')


@login_required(login_url='/ac3app/')
def new_pass(request):
    requesting_user = User.objects.get_by_natural_key(request.user)
    request_userProfile = UserProfile.objects.get(user = requesting_user)
    if request.method == 'POST':
        if 'Cancel' in request.POST:
            return HttpResponseRedirect('/ac3app/profile/')

        name = request.POST.get('chooseuser', None)

        if((requesting_user.is_staff and name == requesting_user.username) or name == None):
            name = requesting_user.username

        user = User.objects.get_by_natural_key(username = name)
        userProfile = UserProfile.objects.get(user = user)
        new = request.POST['nPassword']
        confirmed = request.POST['cPassword']
        if new == confirmed and len(new) > 5:
            user.set_password(new)
            userProfile.hasTempPassword = False
            user.save()
            userProfile.save()

            if(requesting_user.username == name):
                logout(request)
                request.session.flush()
                messages.add_message(request, messages.SUCCESS, 'You Have Successfully Changed Your Password')
                h = HttpResponseRedirect('/ac3app/')
                #TODO: Need to find a way to remove secure logged in user cookie
                h.delete_cookie('logged')
                return h
            else:
                return HttpResponseRedirect('/ac3app/profile/')
        elif new != confirmed:
            messages.add_message(request, messages.ERROR, 'Passwords Do Not Match')
        elif len(new) < 5:
            messages.add_message(request, messages.ERROR, 'Password Must Be Greater than 5 Characters')
        else:
            messages.add_message(request, messages.ERROR, 'Password Is Invalid')
    form = PasswordForm()
    context = {"form": form,
               "profile": request_userProfile
    }
    return render(request, 'ac3app/NewPassword.html', context)


@login_required(login_url='/ac3app/')
def main_view(request):
    form = FilterForm(request.POST)
    latest_events = Event.objects.order_by('-date_created')[:5]
    latest_events_json = serializers.serialize('json', Event.objects.all(), use_natural_foreign_keys=True,
                                               use_natural_primary_keys=True)
    context = {'latest_events': latest_events, 'latest_events_json': latest_events_json, 'form': form}
    return render(request, 'ac3app/MainView.html', context)

def login_view(request):
    if request.method == 'POST':
        if 'login_sub' in request.POST:
            username = request.POST['username']
            password = request.POST['password']

            user = authenticate(username=username, password=password)

            if user:
                if user.is_active:
                    userProfile = UserProfile.objects.get(user=user)
                    login(request, user)
                    request.session.set_expiry(300)
                    if userProfile.hasTempPassword:
                        h = HttpResponseRedirect('/ac3app/newpassword')
                    else:
                        h = HttpResponseRedirect('/ac3app/mainview')
                    #TODO: Figure out a better way to save secure user cookies
                    h.set_cookie('logged', 'temp', max_age=120)
                    return h
                else:
                    return HttpResponse('Account disabled.')
            else:
                messages.add_message(request, messages.ERROR, 'Incorrect username or password!')
                return HttpResponseRedirect('/ac3app/')
                #return render(request, 'ac3app/login.html')
        elif 'forgot_password_sub' in request.POST:
            return emails.forgot_password_email(request)
        else:
            return render(request, 'ac3app/login.html')


@login_required(login_url='/ac3app/login/')
def user_logout(request):
    logout(request)
    request.session.flush()
    messages.add_message(request, messages.SUCCESS, 'You have now successfully logged out.')
    h = HttpResponseRedirect('/ac3app/')
    #TODO: Need to find a way to remove secure logged in user cookie
    h.delete_cookie('logged')
    return h
