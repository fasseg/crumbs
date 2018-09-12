# Fish shell completions for crumbs

function __fish_crumbs_no_subcommand -d 'Test if crumbs has yet to be given the subcommand'
	for i in (commandline -opc)
		if contains -- $i list insert insert-exec show exec edit delete
			return 1
		end
	end
	return 0
end

function __fish_crumbs_use_crumblist -d 'Test if crumb command should have list of crumbs as potential completion'
	for i in (commandline -opc)
		if contains -- $i show exec edit delete
			return 0
		end
	end
	return 1
end

function __fish_crumbs_stop_completion -d 'Test if we need to stop completion'
	if [ (count (commandline -opc)) -ge 3 ]
		return 0
	end
end

complete -c crumbs -n '__fish_crumbs_stop_completion' -x

complete -c crumbs -n '__fish_crumbs_use_crumblist' -x -a "(gfind $HOME/.crumbs -type f -printf '%P\n')" -d 'Crumbs'

complete -c crumbs -s h -l help -d 'Display help and exit'
complete -c crumbs -s c -l config -d 'Use a different configuration file'
complete -c crumbs -s v -l verbose -d 'Print additional information. Useful for debugging'

complete -f -n '__fish_crumbs_no_subcommand' -c crumbs -a 'list' -d 'List the current crumbs'
complete -f -n '__fish_crumbs_no_subcommand' -c crumbs -a 'insert' -d 'Add a new crumb'
complete -f -n '__fish_crumbs_no_subcommand' -c crumbs -a 'insert-exec' -d 'Execute and add a new crumb'
complete -f -n '__fish_crumbs_no_subcommand' -c crumbs -a 'show' -d 'Show a crumb'
complete -f -n '__fish_crumbs_no_subcommand' -c crumbs -a 'exec' -d 'Execute a crumb'
complete -f -n '__fish_crumbs_no_subcommand' -c crumbs -a 'edit' -d 'Edit a crumb'
complete -f -n '__fish_crumbs_no_subcommand' -c crumbs -a 'delete' -d 'Delete a crumb'
