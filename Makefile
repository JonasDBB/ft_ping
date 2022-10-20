NAME = ft_ping
SRCDIR = $(shell pwd)
BUILDDIR = $(SRCDIR)/build

.PHONY: check-and-reinit-submodules

all: $(NAME) check-and-reinit-submodules

check-and-reinit-submodules:
	git config --global --add safe.directory '*'
	@if git submodule status | egrep -q '^[-]|^[+]' ; then \
			git submodule update --init; \
	fi

$(NAME): check-and-reinit-submodules
	@mkdir -p $(BUILDDIR)
	cmake -B$(BUILDDIR) -S$(SRCDIR)
	$(MAKE) -C $(BUILDDIR)

clean:
	$(MAKE) -C $(BUILDDIR) clean

fclean:
	rm -f $(NAME)
	rm -rf $(BUILDDIR)

re: fclean all