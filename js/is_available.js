$(function () {
	$(document).ready(function() {
		/* Admin block settings form */
		$('#edit-settings-options').after($('#delete-setting-button'));
		$('#edit-settings-options').after($('#add-setting-button'));
		$('#edit-settings-options').after($('#configure-setting-button'));


		/* Resource search form*/
		$('input.form-text.hasTimeEntry').css('width', '100% !important');

		$('.form-item-cal-popup-start-date > label').hide();
		$('.form-item-cal-popup-start-date:first-child').css('margin-top', '2px');
		$('.form-item-cal-popup-end-date > label').hide();
		$('.form-item-cal-popup-end-date:first-child').css('margin-top', '2px');

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
				
				if (timeBeforeSameDay(startDay, endDay, startTime, endTime)
						|| dayBefore(startDay, endDay)) {
						input.val(newEndTime);    
				}
				
				var newEndDay = parseInt(startDay[2]) + 1;
				newEndDay = addZero(newEndDay);
				var nextday = startDay[0] + '-' + startDay[1] + '-' + newEndDay;
				
				if (endMin.substr(2) == 'AM' && 
						startTime[1].substr(2) == 'PM') {
						nextdate.val(nextday);
				} else {
						nextdate.val(startdate.val());
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
				if (timeBeforeSameDay(startDay, endDay, startTime, endTime)
						|| dayBefore(startDay, endDay)) {
						
						// set start date/time to one hour before this time
						var startHour = (endTime[0] == 1) ? 12 : endTime[0] - 1;
						startHour = addZero(startHour);
						var startMin = addZero(parseInt(endTime[1]));
						var ampm = endTime[1].substr(2);
						var opposite = (ampm == 'AM') ? 'PM' : 'AM';
						startMin += (endTime[0] == 12) ? opposite : ampm;
						var newStartTime = startHour + ":" + startMin;
						
						var newStartDay = 
								(startHour == 11 && startMin.substr(2) == 'PM') ? // startTime is 11:xxPM
								addZero(parseInt(endDay[2]) - 1) :
								endDay[2];
						var newStartDate = endDay[0] + "-" + endDay[1] + "-" + newStartDay;
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
		
	});
})(jQuery);
