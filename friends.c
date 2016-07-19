#include "friends.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int check_free(const char *name, User *head);

int check_friends(const char *name1, const char *name2, User *head);

int confirm_friends(const User *author, User *target);

void print_pic(const User *user);

void print_friends(const User *user);

void print_posts(const User *user);

void delete_friends(const char *name, User *user);

/*
 * Create a new user with the given name.  Insert it at the tail of the list
 * of users whose head is pointed to by *user_ptr_add.
 *
 * Return:
 *   - 0 if successful
 *   - 1 if a user by this name already exists in this list
 *   - 2 if the given name cannot fit in the 'name' array
 *       (don't forget about the null terminator)
 */
int create_user(const char *name, User **user_ptr_add) {
	int value = 0; // The return value.
	User *user_list = *user_ptr_add;
	if(find_user(name, user_list) != NULL) {
		value = 1;
	} else if(strlen(name) > MAX_NAME - 1) {
		value = 2;
	} else if(user_list == NULL) { // No users in the user list
		User *new_user = malloc(sizeof(User));
		if(new_user == NULL) {
			fprintf(stderr, "Error using malloc, out of memory\n");
			exit(-1);
		}
		*user_ptr_add = new_user;
		strcpy(new_user->name, name);
		new_user->first_post = NULL;
		new_user->next = NULL;
		new_user->profile_pic[0] = '\0';
		for(int i = 0; i < MAX_FRIENDS; i++) { // All users in the friends list is set to NULL.
			new_user->friends[i] = NULL;
		}
	} else {
		User *prev_user;
		while(user_list != NULL) { // If users already exist.
			prev_user = user_list;
			user_list = user_list->next;
		}
		User *new_user = malloc(sizeof(User));
		if(new_user == NULL) {
			fprintf(stderr, "Error using malloc, out of memory\n");
			exit(-1);
		}
		strcpy(new_user->name, name);
		new_user->first_post = NULL;
		new_user->next = NULL;
		prev_user->next = new_user;
		new_user->profile_pic[0] = '\0';
		for(int i = 0; i < MAX_FRIENDS; i++) {
			new_user->friends[i] = NULL;
		}
	}
    return value;
}


/*
 * Return a pointer to the user with this name in
 * the list starting with head. Return NULL if no such user exists.
 *
 * NOTE: You'll likely need to cast a (const User *) to a (User *)
 * to satisfy the prototype without warnings.
 */
User *find_user(const char *name, const User *head) {
	User *current_pt = (User *) head;
	while(current_pt != NULL) {
		if(strcmp(current_pt->name, name) == 0) { // Compares names.
			return current_pt;
		}
		current_pt = current_pt->next;
	}
    return NULL;
}


/*
 * Print the usernames of all users in the list starting at curr.
 * Names should be printed to standard output, one per line.
 */
void list_users(const User *curr) {
	User *current_user = (User *) curr;
	fprintf(stdout, "User List\n" );
	while(current_user != NULL) {
		fprintf(stdout, "    %s\n" ,current_user->name);
		current_user = current_user->next;
	}
}


/*
 * Change the filename for the profile pic of the given user.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the file does not exist.
 *   - 2 if the filename is too long.
 */
int update_pic(User *user, const char *filename) {
	if(strlen(filename) > MAX_NAME - 1) { // Checks the length of the filename.
		return 2;
	} else if(access(filename, F_OK) != -1) { // Checks if the file exists.
		strcpy(user->profile_pic, filename);
		return 0;
	} else {
		return 1;
	}
}


/*
 * Make two users friends with each other.  This is symmetric - a pointer to
 * each user must be stored in the 'friends' array of the other.
 *
 * New friends must be added in the first empty spot in the 'friends' array.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the two users are already friends.
 *   - 2 if the users are not already friends, but at least one already has
 *     MAX_FRIENDS friends.
 *   - 3 if the same user is passed in twice.
 *   - 4 if at least one user does not exist.
 *
 * Do not modify either user if the result is a failure.
 * NOTE: If multiple errors apply, return the *largest* error code that applies.
 */
int make_friends(const char *name1, const char *name2, User *head) {
	if(find_user(name1, head) == NULL || find_user(name2, head) == NULL) {
		return 4;
	} else if(strcmp(name1, name2) == 0) {
		return 3;
	} else if(check_free(name1, head) == -1 || check_free(name2, head) == -1) {
		return 2;
	} else if(check_friends(name1, name2, head) == 0) {
		return 1;
	} else {
		find_user(name1, head)->friends[check_free(name1, head)] = find_user(name2, head);
		find_user(name2, head)->friends[check_free(name2, head)] = find_user(name1, head);
		return 0;
	}
}


/*
 * Print a user profile.
 * For an example of the required output format, see the example output
 * linked from the handout.
 * Return:
 *   - 0 on success.
 *   - 1 if the user is NULL.
 */
int print_user(const User *user) {
    if(user != NULL) { // Checks if the user is not NULL.
    	print_pic(user);
    	fprintf(stdout, "Name: %s\n\n", user->name);
    	fprintf(stdout, "------------------------------------------\n");
    	print_friends(user);
    	fprintf(stdout, "------------------------------------------\n");
    	print_posts(user);
    	fprintf(stdout, "------------------------------------------\n");
    	return 0;
    }
    return 1;
}


/*
 * Make a new post from 'author' to the 'target' user,
 * containing the given contents, IF the users are friends.
 *
 * Insert the new post at the *front* of the user's list of posts.
 *
 * Use the 'time' function to store the current time.
 *
 * 'contents' is a pointer to heap-allocated memory - you do not need
 * to allocate more memory to store the contents of the post.
 *
 * Return:
 *   - 0 on success
 *   - 1 if users exist but are not friends
 *   - 2 if either User pointer is NULL
 */
int make_post(const User *author, User *target, char *contents) {
	if (author == NULL || target == NULL) {
		free(contents);
		return 2;
	} else if(confirm_friends(author, target) == 1) {
		free(contents);
		return 1;
	} else {
		Post *new_post = malloc(sizeof(Post));
		if(new_post == NULL) { // Check if malloc fails.
			fprintf(stderr, "Error using malloc, out of memory\n");
			exit(-1);
		}
		strcpy(new_post->author, author->name);
		new_post->contents = contents;
		time_t *curr_time = malloc(sizeof(time_t*));
		if(curr_time == NULL) {
			fprintf(stderr, "Error using malloc, out of memory\n");
			exit(-1);
		}
		*curr_time = time (NULL); // Sets the time.
		new_post->date = curr_time;
		new_post->next = target->first_post;
		target->first_post = new_post;
		return 0;
	}
}


/*
 * From the list pointed to by *user_ptr_del, delete the user
 * with the given name.
 * Remove the deleted user from any lists of friends.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if a user with this name does not exist.
 */
int delete_user(const char *name, User **user_ptr_del) {
	User *current_user = *user_ptr_del;
	User *prev_user = NULL;
	Post *post;
	while(current_user != NULL) {
		if(strcmp(current_user->name, name) == 0) {
			if(prev_user == NULL) {
				*user_ptr_del = current_user->next;
			} else {
				prev_user->next = current_user->next;
			}
			post = current_user->first_post;
			while(post != NULL) {
				current_user->first_post = post->next;
				free(post->contents);
				free(post->date);
				free(post);
				post = current_user->first_post;
			}
			free(current_user);
			delete_friends(name, *user_ptr_del); // Deletes users off friends lists.
			return 0;
		}
		prev_user = current_user;
		current_user = current_user->next;
	}
    return 1;
}


/*
 * Return a free index for the user to add a friend,
 * -1 if no free index exists.
 *
 * Return:
 *   - -1 if the user has max friends.
 *   - an index where a friend can be added.
 */
int check_free(const char *name, User *head) {
	User *user = find_user(name, head);
	for(int i = 0; i < MAX_FRIENDS; i++) {
		if(user->friends[i] == NULL) { // NULL means free space.
			return i;
		}
	}
	return -1;
}


/*
 * Return whether or not the two users are friends,
 * 0 if they are 1 if they are not.
 *
 * Return:
 *   - 0 if the users are friends.
 *   - 1 if the users are not friends.
 */
int check_friends(const char *name1, const char *name2, User *head) {
	User *user = find_user(name1, head);
	for(int i = 0; i < MAX_FRIENDS; i++) {
		if(user->friends[i] != NULL && strcmp(user->friends[i]->name, name2) == 0) {
			return 0;
		}
	}
	return 1;
}

/*
 * Return whether or not the two users are friends,
 * 0 if they are 1 if they are not.
 *
 * Return:
 *   - 0 if the users are friends.
 *   - 1 if the users are not friends.
 */
int confirm_friends(const User *author, User *target) {
	for(int i = 0; i < MAX_FRIENDS; i++) {
		if(target->friends[i] != NULL && strcmp(author->name, target->friends[i]->name) == 0) {
			return 0;
		}
	}
	return 1;
}

/*
 * Print a user's profile picture.
 */
void print_pic(const User *user) {
	if(strcmp(user->profile_pic, "") != 0) {
		FILE *file;
		char line[256];
		file = fopen(user->profile_pic, "r");
		while(fgets(line, 255, file) != NULL) {
			fprintf(stdout, "%s", line);
		}
		fprintf(stdout, "\n\n");
	}
}

/*
 * Print a user's friends.
 */
void print_friends(const User *user) {
	fprintf(stdout, "Friends:\n");
	for(int i = 0; i < MAX_FRIENDS; i++) { // Goes through each friend.
		if(user->friends[i] != NULL) {
			fprintf(stdout, "%s\n", user->friends[i]->name);
		}
	}
}

/*
 * Print posts on a user's profile.
 */
void print_posts(const User *user) {
	fprintf(stdout, "Posts:\n");
	Post *current_post = user->first_post;
	struct tm *curr_time;
	while(current_post != NULL) { // Goes through each post.
		fprintf(stdout, "From: %s\nDate: ", current_post->author);
		curr_time = localtime(current_post->date);
		fputs(asctime(curr_time), stdout);
		fprintf(stdout, "\n");
		fprintf(stdout, "%s\n", current_post->contents);
		current_post = current_post->next;
		if (current_post != NULL) {
			fprintf(stdout, "\n===\n\n");
		}
	}
}

/*
 * Delete any user with the given name from all user's friends list.
 */
void delete_friends(const char *name, User *user) {
	User *curr_user = user;
	while(curr_user != NULL) { // Checks all friends lists.
		for(int i = 0; i < MAX_FRIENDS; i++) { // Checks each friend.
			if(curr_user->friends[i] != NULL && strcmp(curr_user->friends[i]->name, name) == 0) {
				curr_user->friends[i] = NULL;
			}
		}
		curr_user = curr_user->next;
	}
}
