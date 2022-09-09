NAME = ft_ping
SRCDIR = $(shell pwd)
BUILDDIR = $(SRCDIR)/build

all: $(NAME)

$(NAME):
	@mkdir -p $(BUILDDIR)
	cmake -B$(BUILDDIR) -S$(SRCDIR)
	$(MAKE) -C $(BUILDDIR)

clean:
	$(MAKE) -C $(BUILDDIR) clean

fclean:
	rm -f $(NAME)
	rm -rf $(BUILDDIR)

re: fclean all