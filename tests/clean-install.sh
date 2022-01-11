# Remove user-specific gems and git repos
rm -rf ~/.bundle/ ~/.gem/bundler/ ~/.gems/cache/bundler/

# Remove system-wide git repos and git checkouts
rm -rf $GEM_HOME/bundler/ $GEM_HOME/cache/bundler/

# Remove project-specific settings
rm -rf .bundle/

# Remove project-specific cached gems and repos
rm -rf vendor/cache/

# Remove the saved resolve of the Gemfile
rm -rf Gemfile.lock

# Uninstall the rubygems-bundler and open_gem gems
rvm gemset use global # if using rvm
gem uninstall rubygems-bundler open_gem

# Try to install one more time
bundle install