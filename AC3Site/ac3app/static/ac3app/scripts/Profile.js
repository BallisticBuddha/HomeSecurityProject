$(document).ready(function() {
    $("#bEdit").click(function () {
        $(".info").toggle();
        $(".edit").toggle();
        $("#userselect").prop('disabled', true)

        $("#ifirst").val($("#lfirst").text())
        $("#ilast").val($("#llast").text())
        $("#ipin").val($("#lpin").text())
        $("#iemail").val($("#lemail").text())
        $("#iphone").val($("#lphone").text())
    });

    $("#bNew").click(function () {
        $(".info").toggle();
        $(".edit").toggle();
        $(".id").hide();
        $(".new").show();
        $("#userselect").toggle();
        $("#pass").toggle();
        $("#save").toggle();
    });

    $("#bDelete").click(function () {
        var r = confirm("Are you sure you want to delete User: ");
        if (r == true) {
            return true;
        } else {
            return false;
        }
    });


    $("#bCancel").click(function () {
        $(".info").toggle();
        $(".edit").toggle();
        if($(".new").css('display') != 'none')//Check if cancelling create user
        {
            $("#save").toggle();
            $("#pass").toggle();
            $(".new").toggle();
            $("#userselect").toggle();
        }
        $("#userselect").prop('disabled', false)

        $("#ifirst").val('');
        $("#ilast").val('');
        $("#ipin").val('')
        $("#iemail").val('')
        $("#iphone").val('')
        return false;
    });
});

function Userselect(){
    $("select option:selected").each(function () {
        $("#name").val($(this).val());
        var s = {'name': $(this).val()}
        jQuery.ajax({
            type: "GET",
            url: '../profileAjax/',
            dataType: "text",
            data: s,
            beforesend: function(){
                $("#bEdit").prop('disabled', true)
            },
            success: function(data) {
                data = JSON.parse(data);
                fillUserData(data);
                $("#bEdit").prop('disabled', false)
            }
        })
    });
}

function fillUserData(data){
    $("#id").text(data.id)
    $("#isStaff").text(data.is_staff)
    $("#lfirst").text(data.first)
    $("#llast").text(data.last)
    $("#lpin").text(data.pin)
    $("#lemail").text(data.email)
    $("#lphone").text(data.phone)
}

