$(function () {
	$(document).ready(function() {
	
		/* Admin block settings form */
		$('#edit-settings-options').after($('#delete-setting-button'));
		$('#edit-settings-options').after($('#add-setting-button'));
		$('#edit-settings-options').after($('#configure-setting-button'));
		$('div[id^="configure-"]').attr('style', 'clear: both;');

		/* Hide previous form stage elements */
		Drupal.behaviors.is_available = {
			attach: function (context, settings) {
			 
				$("div#configure-resource-type").mouseenter(function() {
						if ($(this).find('div').length > 1) {
								$(this).attr('style', 'clear: both;');
								$(this).prev('#configure-title-entity-type').hide();
								$('div#console').children('.messages.error').hide();
						}
				});
				$("div#configure-resource-type").mouseleave(function() {
						 $(this).parent().children('div.messages.error').hide();
				});
				

				$("div#configure-building").mouseenter(function() {
						if ($(this).find('fieldset').length > 0) {
								$(this).parent().find('p#step-2').hide();
								$(this).parent().find('#resource-types').hide();
								$(this).parent().find('div[style="display: block"]').hide();
								$('div#console').children('.messages.error').hide();
						}
				});
				$("div#configure-building").mouseleave(function() {
						$(this).parent().find('div.messages.error').hide();
				});
				

				$("div#configure-auth-building").mouseenter(function() {
						if ($(this).find('fieldset').length > 0) {
								$(this).parent().parent().find('p#step-3').hide();
								$(this).prev('fieldset').hide();
								$('div#console').children('.messages.error').hide();
						}
				});
				$("div#configure-auth-building").mousedown(function() {
						$(this).parent().find('div.messages.error').hide();
				});
				
				$("div#configure-entref").mouseenter(function() {
						if ($(this).find('fieldset').length > 0) {
								$(this).parent().parent().find('p#step-4').hide();
								$(this).parent().find('fieldset[id^="edit-is-available-auth-building-"]').hide();
								$(this).parent().find('fieldset[id^="edit-is-available-content-type-"]').hide();
								$('div#console').children('.messages.error').hide();
						}
				});
				$("div#configure-entref").mousedown(function() {
						$(this).parent().find('div.messages.error').hide();
				});
				
				$("div#configure-date-table").mouseenter(function() {
						if ($(this).find('fieldset').length > 0) {
								$(this).parent().find('p#step-5').hide();
								$(this).parent().find('fieldset[id^="edit-entref-"]').hide();
								$('div#console').children('.messages.error').hide();
						}
				});
				$("div#configure-date-table").mousedown(function() {
						$(this).parent().find('div.messages.error').hide();
				});
				
				$("div#edit-actions").mouseenter(function() {
						if ($(this).find('input#edit-submit').length > 0) {
								$(this).parent().find('p#step-6').hide();
								$(this).parent().find('fieldset[id^="edit-is-available-date-"]').hide();
								$('div#console').children('.messages.error').hide();
						}
				});
				$("div#edit-actions").mousedown(function() {
						$(this).parent().children('div.messages.error').hide();
				});
			}
		};


		/* Resource search form*/
		// CSS for the datepicker/time fields
		$('.cal-popup-start').css('width', '100%');
		$('.form-item-cal-popup-start').css('width','100%');
		$("div[id^='edit-cal-popup-start']").css('width', '100%');
		$('.cal-popup-end').css('width', '100%');
		$('.form-item-cal-popup-end').css('width','100%');
		$("div[id^='edit-cal-popup-end']").css('width', '100%');
		$('div[id^="available-resources-"]').find('p').attr("style", "clear: both;");

		// Resize time fields and hide description label
		$('input.form-text.hasTimeEntry').css('width', '100% !important');
		var sTimeWrap = $(".form-item-cal-popup-start-time");
		var eTimeWrap = $(".form-item-cal-popup-end-time");
		sTimeWrap.find("input[id^='edit-cal-popup-start-']").attr('size', '7');
		eTimeWrap.find("input[id^='edit-cal-popup-end-']").attr('size', '7');
		eTimeWrap.parent().find('.description').hide();

		// Hide date labels
		$('.form-item-cal-popup-start-date > label').hide();
		$('.form-item-cal-popup-start-date:first-child').css('margin-top', '2px');
		$('.form-item-cal-popup-end-date > label').hide();
		$('.form-item-cal-popup-end-date:first-child').css('margin-top', '2px');

		// Resize the date/time fields
		$('.form-item-cal-popup-start-time').each(function() {
		$(this).parent().prev().attr("style", "width: 50%; margin-bottom: -2px;");
		$(this).prev().find("input[name='cal_popup_start[date]']").attr("style", "height: 27px !important");
		$(this).find("input[id^='edit-cal-popup-start-']").attr("style", "height: 27px !important");
		$(this).parents().eq(3).next().find("input[class='hasDatePicker']").attr("style", "height: 27px !important");
		$(this).parents().eq(3).next().find("input[name='cal_popup_end[time]']").attr("style", "height: 27px !important");
		var region = $(this).parents().eq(9);
		if (region.attr('class').substr(0, 21) == 'region region-sidebar') {
				region.find('.form-item-cal-popup-start-time').attr('style', 'margin: -15px 0 0 0px !important');
				region.find('.form-item-cal-popup-end-time').attr('style', 'margin: -16px 0 0 0px !important');
		} else if (region.attr('class') == 'region region-content') {
				region.find('.form-item-cal-popup-start-time').attr('style', 'margin: -17px 0 0 0px !important');
				region.find('.form-item-cal-popup-end-time').attr('style', 'margin: -19px 0 0 0px !important');
		}
		});



		/* Start date/time onblur handler */
		$("input[id^='edit-cal-popup-start-']").focusout(function() {
				var parent = $(this).parent().parent().parent().parent().parent();
				if ($(this).attr('name') == 'cal_popup_start[time]') 
						parent = parent.parent();
				var next = parent.next();
			 var inner = next.find(".form-item-cal-popup-end-time"); //"input[id^='edit-cal-popup-start-']");
			 var input = inner.find("input[id^='edit-cal-popup-end-']"); //'input#edit-cal-popup-end-timeEntry-popup-1');
			 var theTime = parent.find('.hasTimeEntry');

				var startTime = theTime.val().split(':');
				var endTime = input.val().split(':');
				var sdate = parent.find('.form-item-cal-popup-start-date');
				var startdate = sdate.find("input[id^='edit-cal-popup-start-']");
				var ndate = next.find('.form-item-cal-popup-end-date');
				var nextdate = ndate.find("input[id^='edit-cal-popup-end-']");
				var startDay = startdate.val().split('-');
				var endDay = nextdate.val().split('-');
				
				var endHours = parseInt(startTime[0]) + 1;
				if (endHours > 12) endHours = (endHours - 12);
				endHours = addZero(endHours);
				var ampm = startTime[1].substr(2);
				var opposite = (ampm == 'AM') ? 'PM' : 'AM';
				var endMin = parseInt(startTime[1]);
				endMin = addZero(endMin);
				endMin += (endHours == 12) ? opposite : ampm;
				var newEndTime = endHours + ':' + endMin;
				
				var newEndDay = addDay(parseInt(startDay[2]), startDay[0], startDay[1], 1);
				var nextday = startDay[0] + '-' + startDay[1] + '-' + newEndDay;
					 
				
				if (startTime[1].substr(2) == 'PM' && endMin.substr(2) == 'AM') {
						nextdate.val(nextday);
				} else if (timeBeforeSameDay(startDay, endDay, startTime, endTime)
						|| dayBefore(startDay, endDay)) {

						input.val(newEndTime);      
						if (startTime[1].substr(2) == 'PM' && endTime.substr(2) == 'AM') {
								nextdate.val(nextday);
						} else {
								nextdate.val(startdate.val());
						}
				}      
		});

		/* End date/time onblur handler */
		$("input[id^='edit-cal-popup-end-']").focusout(function() {
				var parent = $(this).parent().parent().parent().parent().parent();
				if ($(this).attr('name') == 'cal_popup_end[time]') 
						parent = parent.parent();
				var next = parent.next();
			 var theTime = parent.find('.hasTimeEntry');
		 
				var prev = parent.prev(); // start date
				var sdate = prev.find('.form-item-cal-popup-start-date');
				var startdate = sdate.find("input[id^='edit-cal-popup-start-']");
				var ndate = parent.find('.form-item-cal-popup-end-date');
				var nextdate = ndate.find("input[id^='edit-cal-popup-end-']");
				
				var inner = prev.find(".form-item-cal-popup-start-time"); 
				var input = inner.find("input[id^='edit-cal-popup-start-']");
				
				var startDay = startdate.val().split('-');
				var endDay = nextdate.val().split('-');
				var endTime = theTime.val().split(':');
				var startTime = input.val().split(':'); 
				var sHour = getHour(startTime[0], startTime[1]); 
				var eHour = getHour(endTime[0], endTime[1]);
				var startDate = new Date(startDay[0], startDay[1], startDay[2],
						sHour, parseInt(startTime[1]));
				var endDate = new Date(endDay[0], endDay[1], endDay[2], eHour, 
						parseInt(endTime[1]));

				if (endDate <= startDate) {
						// set start date/time to one hour before this time
						var nsDate = new Date(endDate.valueOf() - 60*60*1000);
						var startHour = nsDate.getHours();
						var startMin = addZero(nsDate.getMinutes());
						if (startHour == 0) startMin += 'AM';
						else if (startHour >= 12) {
								if (startHour > 12) startHour -= 12;
								startMin += 'PM';
						} else startMin += 'AM';
						var newStartTime = addZero(startHour) + ":" + startMin;        
						var newStartDate = nsDate.getFullYear() + "-" + 
								("0" + nsDate.getMonth()).slice(-2) + "-" + 
								("0" + nsDate.getDate()).slice(-2);
						input.val(newStartTime);
						startdate.val(newStartDate);
			 
				}
		});

		// Returns true if endDay is prior to startDay
		function dayBefore(startDay, endDay) {
				if (startDay.length != endDay.length) return false;
				var before = false;
				for (i = 0; i < startDay.length; i++) 
						before = before || (startDay[i] - endDay[i] > 0);
				return before;
		}


		function timeBeforeSameDay(startDay, endDay, startTime, endTime) {
				if (startDay.length != endDay.length ||
						startTime.length != endTime.length) 
						return false;
				var bool = datesEqual(startDay, endDay) && 
						(hourBefore(startTime, endTime) || minBefore(startTime, endTime));
				return bool;
		}

		// Returns true if startDay is the same date as endDay
		function datesEqual(startDay, endDay) {
				if (startDay.length != endDay.length) return false;
				var equals = true; var i;
				for (i = 0; i < startDay.length; i++) 
						equals &= (startDay[i] == endDay[i]);
				return equals;
		}

		// Returns true if the endTime hr precedes startTime's hr
		function hourBefore(startTime, endTime) {
				if (startTime.length != endTime.length) return false;
				var sampm = startTime[1].substr(2);
				var eampm = endTime[1].substr(2);
				var shr = startTime[0];
				var ehr = endTime[0];
				var bool = (sampm == eampm && // same am/pm
						(shr - ehr > 0 && shr != 12) ||
						(ehr == 12 && shr != 12)) ||
						(sampm == 'PM' && eampm == 'AM');
				return bool;
		}

		// Returns true if the endTime min precedes startTime's min
		function minBefore(startTime, endTime) {
				if (startTime.length != endTime.length) return false;
				var startMin = parseInt(startTime[1]);
				var endMin = parseInt(endTime[1]);
				var bool = (startTime[0] - endTime[0] == 0 && // same hour, same am/pm
						startTime[1].substr(2) == endTime[1].substr(2) &&
						startMin - endMin >= 0);
				return bool;
		}

		function addZero(x) {
				var ret = (x < 10) ? "0" + x : x;
				return ret;
		}

		function addDay(day, year, month, value) {
				var newDate = new Date(year, month, day+value);
				return addZero(newDate.getDate());
		}

		function getHour(str_hr, str_min) {
				var hr = parseInt(str_hr);
				if (str_min.substr(2) == 'PM' && hr != 12) 
						hr += 12;
				else if (str_min.substr(2) == 'AM' && hr == 12)
						hr = 0;
				return hr;
		}
		
	});
})(jQuery);
