jQuery(document).ready(function(){
	var $ = jQuery;								
	$('.post-content img').load(function(){
		if(this.width > 730){
			$(this).attr('width',730).removeAttr('height');
					if( 0 == $(this).parents('a').size() ){
				$(this).wrap($('<a target="_blank" />').attr('href',this.src));	
			};
		}
	});						   
});

