$(function () {
	$(document).ready(function() {
		/* Admin settings form theming */
		
$('#edit-settings-options').after($('#delete-setting-button'));
$('#edit-settings-options').after($('#add-setting-button'));
$('#edit-settings-options').after($('#configure-setting-button'));


	/* Actual form theming will go here */
$(this).on("click", '#configure-resource-type', function(event) {
    alert("Clicked");
});
$(this).on("click", '#configure-building', function(event) {
    alert("Clicked");
});


$('input.form-text.hasTimeEntry').css('width', '100% !important');

$('.form-item-cal-popup-start-date > label').hide();
$('.form-item-cal-popup-start-date:first-child').css('margin-top', '2px');


$('input#edit-cal-popup-start-datepicker-popup-0').focusout(function() {
    alert('asdfsadfsdfa');
});
$("input[id^='edit-cal-popup-start-']").focusout(function() {
    var parent = $(this).parent().parent().parent().parent().parent().parent();
// alert(parent.attr('class'));
    var next = parent.next();
   var end = next.find('span.timeEntry_wrap'); //"input[id^='edit-cal-popup-start-']");
    alert(end.attr('class'));
    alert(end.val());    
});



$('.form-item-cal-popup-end-date > label').hide();
$('.form-item-cal-popup-end-date:first-child').css('margin-top', '2px');
		
		
		
	});
})(jQuery);
