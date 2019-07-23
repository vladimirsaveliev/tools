#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct frame {
	struct frame *next;
	char line[0];
};

struct bt {
	struct bt *next;
	int nr;
	struct frame *frames;
	char **pidline;
	int max;
};

struct bt *bts = NULL;

/*
 * line is something like
 * PID: 120891  TASK: ffff880781d82ae0  CPU: 5   COMMAND: "sleep"
 */
static struct bt *new_bt(const char *pidline)
{
	struct bt *bt;

	bt = malloc(sizeof(struct bt));
	if (bt == NULL) {
		perror("malloc failed");
		exit(1);
	}
	bt->pidline = malloc(sizeof(bt->pidline[0]) * 10);
	if (bt->pidline == NULL) {
		perror("malloc failed");
		exit(1);
	}
	bt->max = 10;

	bt->pidline[0] = malloc(strlen(pidline) + 1);
	if (bt->pidline[0] == NULL) {
		perror("malloc failed");
		exit(1);
	}
	strcpy(bt->pidline[0], pidline);
	bt->nr = 1;
	
	bt->frames = NULL;

	return bt;
}

/*
 * line is
 * #1 [ffff8801c226dd08] ptlrpc_wait_event at ffffffffa13e09b5 [ptlrpc]
 * or
 * #3 [ffff8801c226dee8] kthread at ffffffff8109ac66
 * ignore others which do not have # as second char
 */
static void add_frame(struct bt *bt, char *line)
{
	struct frame *new_frame;
	char *p;

	if (bt == NULL) {
		printf("no bt\n");
		exit(1);
	}

	if (line[0] != '#' && line[1] != '#')
		return;

	p = malloc(sizeof(struct frame) + strlen(line) + 1);
	if (p == NULL) {
		perror("kmalloc failed");
		exit(1);
	}
	new_frame = (struct frame *)p;
	new_frame->next = bt->frames;
	bt->frames = new_frame;
	strcpy(new_frame->line, line);
}

/* return 1 if bts are identical */
static int bt_cmp(const struct bt *bt1, const struct bt *bt2)
{
	struct frame *fr1, *fr2;

	fr1 = bt1->frames;
	fr2 = bt2->frames;
	while (fr1 && fr2) {
		if (strncmp(fr1->line, fr2->line, 5) ||
		    strcmp(fr1->line + 23, fr2->line + 23))
			return 0;

		fr1 = fr1->next;
		fr2 = fr2->next;
	}
	return (fr1 == NULL && fr2 == NULL);
}

static void print_bt(const struct bt *bt);

static struct bt *is_bt_uniq(const struct bt *new_bt)
{
	struct bt *bt;

	if (new_bt == NULL) {
		printf("no bt\n");
		exit(1);
	}

	bt = bts;
	while (bt) {
		if (bt_cmp(bt, new_bt)) {
			return bt;
		}
		bt = bt->next;
	}
	return NULL;
}

static void save_bt(struct bt *bt)
{
	bt->next = bts;
	bts = bt;
}

static void free_bt(struct bt *bt)
{
	struct frame *frame, *next;
	int i;

	frame = bt->frames;
	while (frame) {
		next = frame->next;
		free(frame);
		frame = next;
	}
	for (i = 0; i < bt->nr; i ++)
		free(bt->pidline[i]);
	free(bt);
}

static void handle_bt(struct bt *bt)
{
	struct bt *found;

	if (bt == NULL) {
		printf("no bt\n");
		exit(1);
	}

	found = is_bt_uniq(bt);
	if (found == NULL) {
		save_bt(bt);
	} else {
		/* add line to array found->pidline */
		if (found->nr == found->max) {
			/* realloc array to pidlines */
			printf("nr %d, max %d\n", found->nr, found->max);
			found->pidline = realloc(found->pidline,
						 sizeof(found->pidline[0]) * (found->max + 10));
			if (found->pidline == NULL) {
				perror("realloc failed");
				exit(1);
			}
			found->max += 10;
		}
		found->pidline[found->nr] = malloc(strlen(bt->pidline[0]) + 1);
		if (found->pidline[found->nr] == NULL) {
			perror("malloc failed");
			exit(1);
		}
		strcpy(found->pidline[found->nr], bt->pidline[0]);
		found->nr ++;

		free_bt(bt);
	}
}

static void print_frame(struct frame *frame)
{
	if (frame == NULL)
		return;
	print_frame(frame->next);
	printf("%s", frame->line);
}

static void print_bt(const struct bt *bt)
{
	int i;

	printf("### %d ###\n", bt->nr);
	for (i = 0; i < bt->nr; i ++)		
		printf("%s", bt->pidline[i]);

	print_frame(bt->frames);
	printf("-----\n\n");
}

static void list_bts(void)
{
	struct bt *bt;

	bt = bts;
	while (bt) {
		print_bt(bt);
		bt = bt->next;
	}
}

int main(int argc, char **argv)
{
	FILE *f;
	char *line;
	size_t n;
	int rc;
	struct bt *bt;

	if (argc != 2) {
		printf("usage: %s backtrace\n", argv[0]);
		return 1;
	}

	f = fopen(argv[1], "r");
	if (f == NULL) {
		perror("fopen failed");
		return 1;
	}

	line = NULL;
	n = 0;
	bt = 0;
	while (1) {
		rc = getline(&line, &n, f);
		if (rc == -1)
			break;

		if (line[0] == '\n') {
			/* end of a process stress, check whethre it
			 * is uniq, add to list if if it not */
			handle_bt(bt);
			continue;
		}
		if (!strncmp(line, "PID:", 4)) {
			/* new backtrace starts */
			bt = new_bt(line);
			continue;
		}
		add_frame(bt, line);
	}
	handle_bt(bt);

	free(line);
	fclose(f);

	list_bts();

	return 0;
}
