<?php 
global $user;
$node = $variables['node'];
$account = $variables['account'];
$uid = $account->uid;
$nrOfForumPosts = $variables['nrOfForumPosts'];
$nrOfComments = $variables['nrOfComments'];
$dateOfLastForumPost = $variables['dateOfLastForumPost'];
$dateOfLastComment = $variables['dateOfLastComment'];
?>

<!--<?php echo "<pre>";
print_r($variables);
echo "</pre>";
?>-->

<?php if (is_array($user->roles) && in_array('administrator', $user->roles)) { ?>
<?php /*Code parts taken from user_stats module */ ?>
	<div class="messages error">
		<h3>Admin Info</h3>
		<span class="tabulator">Userid:</span> <?php print($uid); ?><br/>
		<span class="tabulator">User:</span> <?php echo l($account->name,'user/'.$node->uid);?><br />
		<span class="tabulator">Account created on:</span> <?php print(format_date($account->created)); ?><br />
		<span class="tabulator">Days ago:</span> <?php print floor((time() - $account->created) / 86400); ?><br />
		<span class="tabulator">Last login:</span> <?php print format_date($account->login); ?><br />
		<span class="tabulator">Nr of comments:</span> <?php print $nrOfComments; ?><br />
		<span class="tabulator">Forum posts:</span> <?php print $nrOfForumPosts; ?><br/>
		<span class="tabulator">Last forum post:</span> <?php print format_date($dateOfLastForumPost); ?><br />
		<span class="tabulator">Last comment:</span> <?php print format_date($dateOfLastComment); ?><br />
		<!-- <span class="tabulator">TEST</span><?php print l('REMOVE', 'remove/user/uid/'. $account->uid); ?><br /> -->
		<span class="tabulator">Delete:</span><?php print l('Delete user and content', 'remove/user/uid/confirm/'. $account->uid); ?><br />
	</div>
<?php } ?>