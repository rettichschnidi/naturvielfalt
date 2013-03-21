<?php 
/**
* @file
* Template for Admin User Details
* in Topic View of Forum section.
* 
* Available variables:
* -$node: Node that represents this topic.
* -$account: Account that created the node.
* -$nrOfForumPosts: Number of Forum posts that the user account made.
* -$nrOfComments: Number of Comments that the user account made.
* -$dateOfLastForumPost: Date of last forum post that the user acount made.
* -$dateOfLastComment: Date of last comment that the user acount made.
* -$usernameappears: Number of times the username appears in the StopForumSpam DB.
* -$emailappears: Number of times the email address that was used by user account to sign up
*  appears in the StopForumSpam DB.
* @see remove_user_theme(), remove_user_node_view()
*/
?>

<?php 
global $user;
$node = $variables['node'];
$account = $variables['account'];
$uid = $account->uid;
$nrOfForumPosts = $variables['nrOfForumPosts'];
$nrOfComments = $variables['nrOfComments'];
$dateOfLastForumPost = $variables['dateOfLastForumPost'];
$dateOfLastComment = $variables['dateOfLastComment'];
$roles = $account->roles;
$usernameappears = $variables['usernameappears'];
$emailappears = $variables['emailappears'];
$spam_username_class = $usernameappears > 0 ? 'spam_appears_1': 'spam_appears_0';
$spam_mail_class = $emailappears > 0 ? 'spam_appears_1': 'spam_appears_0';
?>

<?php if (is_array($user->roles) && in_array('administrator', $user->roles)) { ?>
	<div class="messages error">
		<h3><?php echo t('Admin Info'); ?></h3>
		<span class="tabulator"><?php echo t('Userid')?>:</span> <?php print($uid); ?><br/>
		<span class="tabulator"><?php echo t('User')?>:</span> <?php echo l($account->name,'user/'.$node->uid);?><br />
		<span class="tabulator"><?php echo t('Roles')?>:</span><?php $i = 0; $arr_count = count($roles); foreach($roles as $role){ if($i > 0){ echo "<br />"; } echo $role; if($i > 0){ echo "<span class=\"tabulator\">&nbsp;</span>"; } $i++; } ?> <br/>
		<span class="tabulator"><?php echo t('Account created on')?>:</span> <?php print(format_date($account->created)); ?><br />
		<span class="tabulator"><?php echo t('Days ago')?>:</span> <?php print floor((time() - $account->created) / 86400); ?><br />
		<span class="tabulator"><?php echo t('Last login')?>:</span> <?php print format_date($account->login); ?><br />
		<span class="tabulator"><?php echo t('Nr of comments')?>:</span> <?php print $nrOfComments; ?><br />
		<span class="tabulator"><?php echo t('Forum posts')?>:</span> <?php print $nrOfForumPosts; ?><br/>
		<span class="tabulator"><?php echo t('Last forum post')?>:</span> <?php print format_date($dateOfLastForumPost); ?><br />
		<span class="tabulator"><?php echo t('Last comment')?>:</span> <?php print format_date($dateOfLastComment); ?><br />
		<hr class="hr_style"/>
		<span class="tabulator"><?php echo t('Username appears')?>:</span> <div class="appearsWrapper"><div class="<?php print $spam_mail_class; ?>">&nbsp;</div><?php print check_plain($usernameappears)?> (<?php print l('StopForumSpam','http://www.stopforumspam.com/search/'. check_plain($account->name)); ?>)</div>
		<span class="tabulator"><?php echo t('Email appears')?>:</span> <div class="appearsWrapper"><div class="<?php print $spam_username_class; ?>">&nbsp;</div><?php print check_plain($emailappears)?> (<?php print l('StopForumSpam','http://www.stopforumspam.com/search/'. check_plain($account->init)); ?>)</div>
		<span class="tabulator"><?php echo t('Purge')?>:</span><?php print l('Delete user and content', 'remove/user/uid/confirm/'. $account->uid); ?><br />
	</div>
<?php } ?>