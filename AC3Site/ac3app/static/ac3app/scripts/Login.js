$(document).ready(function(){
  $("#f_link").click(function(){
    $(".hfields").toggle();
  });

  $("#bsendemail").click(function(){
	var email = $("#email").val();
	if(!isValidEmailAddress(email))
	{
		$("#error").show();
		return false;
	}
	else
		$("#error").hide();
		$("#emailconf").show();
  });
});