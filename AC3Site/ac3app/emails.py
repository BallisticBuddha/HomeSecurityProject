from django.core.exceptions import ObjectDoesNotExist
from django.core.mail import EmailMessage
from django.contrib.auth.models import User, BaseUserManager
from django.contrib import messages
from django.http import HttpResponseRedirect
from ac3app.models import UserProfile


def forgot_password_email(request):
    if request.method == 'POST':
        try:
            u = User.objects.get(username=request.POST['username'])
            userProfile = UserProfile.objects.get(user = u)
        except ObjectDoesNotExist:
            u = None

        if u is not None:
            tempPass = BaseUserManager.make_random_password(u)
            u.set_password(tempPass)
            userProfile.hasTempPassword = True
            userProfile.save()
            u.save()
            subject = 'AC3 Forgotten Password Request'
            message = 'User: {0}\n You have requested to reset your password\nYour temporary password is: {1}' \
                      ''.format(u.username, tempPass)
            EmailMessage(subject, message, to=[u.email]).send(fail_silently=True)
            messages.add_message(request, messages.SUCCESS, 'An email has been sent!')
            return HttpResponseRedirect('/ac3app/')
        else:
            messages.add_message(request, messages.ERROR, 'The user {0} could not be found'
                                 .format(request.POST['username']))
            return HttpResponseRedirect('/ac3app/')
