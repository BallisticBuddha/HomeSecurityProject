from django.shortcuts import render
from django.core.exceptions import ObjectDoesNotExist
from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.http import HttpResponse
from django.contrib.auth.hashers import make_password
from ac3app.models import UserProfile
import json

@login_required(login_url='/ac3app/')
def profile_view(request):
    user = User.objects.get_by_natural_key(request.user)
    userProfile = UserProfile.objects.get(user = user)
    if(user.is_staff):
        userslist = User.objects.all()
        userslist = list(userslist)
    else:
        userslist = None

    if request.method == 'POST':
        if 'save' in request.POST:
            postname = request.POST['name']
            if(user.is_staff and user.username != postname):
                postuser = User.objects.get_by_natural_key(username=postname)
                postuserProfile = UserProfile.objects.get(user = postuser)
                postuser.first_name = request.POST['firstname']
                postuser.last_name = request.POST['lastname']
                postuser.email = request.POST['email']
                postuserProfile.user_pin = request.POST['userpin']
                postuserProfile.phone_number = request.POST['phonenumber']
                postuserProfile.save()
                postuser.save()
            else:
                user.first_name = request.POST['firstname']
                user.last_name = request.POST['lastname']
                user.email = request.POST['email']
                userProfile.user_pin = request.POST['userpin']
                userProfile.phone_number = request.POST['phonenumber']
                userProfile.save()
                user.save()
        if 'create' in request.POST:
            newUser = User(
                username = request.POST['username'],
                password = make_password(request.POST['password']),
                first_name = request.POST['firstname'],
                last_name = request.POST['lastname'],
                email = request.POST['email']
            )
            newUser.save()
            newUserProfile = UserProfile(
                user = newUser,
                user_pin = request.POST['userpin'],
                phone_number = request.POST['phonenumber']
            )
            newUserProfile.save()
            userslist = User.objects.all()
            userslist = list(userslist)

        if 'delete' in request.POST:
            (User.objects.get(username=request.POST['userselect'])).delete()
            userslist = User.objects.all()
            userslist = list(userslist)

    context = {'user': user,
               'userProfile': userProfile,
               'userslist': userslist
               }
    return render(request, 'ac3app/profile.html', context)


def profileAjax(request):
    if request.method == "GET":
        try:
            user = User.objects.get_by_natural_key(username = request.GET['name'])
            userProfile = UserProfile.objects.get(user = user)

            data = {
            'id': user.id,
            'is_staff': user.is_staff,
            'first': user.first_name,
            'last': user.last_name,
            'email': user.email,
            'pin': userProfile.user_pin,
            'phone': userProfile.phone_number
            }
            return HttpResponse(json.dumps(data), content_type="application/json")
        except ObjectDoesNotExist:
            return HttpResponse("User not found")

