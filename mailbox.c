#define mailbox_c

#include "mailbox.h"

#define NO_MAILBOXES 30

static void *shared_memory = NULL;
static mailbox *freelist = NULL; /* list of free mailboxes.  */

/*
 *  initialise the data structures of mailbox.  Assign prev to the
 *  mailbox prev field.
 */

static mailbox *mailbox_config(mailbox *mbox, mailbox *prev)
{
  // initialsing the in and out veriables that are in mailbox.h
  // inisilising them to 0
  mbox->in = 0;
  mbox->out = 0;
  mbox->prev = prev;
  // initialsing semaphores that are used to monitoring processor
  mbox->item_available = multiprocessor_initSem(0);
  // setting space aviable to max mail box data which is 100
  mbox->space_available = multiprocessor_initSem(MAX_MAILBOX_DATA);
  mbox->mutex = multiprocessor_initSem(1);
  return mbox;
}

/*
 *  init_memory - initialise the shared memory region once.
 *                It also initialises all mailboxes.
 */

static void init_memory(void)
{
  if (shared_memory == NULL)
  {
    mailbox *mbox;
    mailbox *prev = NULL;
    int i;
    _M2_multiprocessor_init();
    shared_memory = multiprocessor_initSharedMemory(NO_MAILBOXES * sizeof(mailbox));
    mbox = shared_memory;
    for (i = 0; i < NO_MAILBOXES; i++)
      prev = mailbox_config(&mbox[i], prev);
    freelist = prev;
  }
}

/*
 *  init - create a single mailbox which can contain a single triple.
 */

mailbox *mailbox_init(void)
{
  mailbox *mbox;

  init_memory();
  if (freelist == NULL)
  {
    printf("exhausted mailboxes\n");
    exit(1);
  }
  mbox = freelist;
  freelist = freelist->prev;
  return mbox;
}

/*
 *  kill - return the mailbox to the freelist.  No process must use this
 *         mailbox.
 */

mailbox *mailbox_kill(mailbox *mbox)
{
  mbox->prev = freelist;
  freelist = mbox;
  return NULL;
}

/*
 *  send - send (result, move_no, positions_explored) to the mailbox mbox.
 */
// Taking first parmeter which is mailbox pointer,
// result of of the search value that is intager value, move number that was used for searching, number of positon explored

void mailbox_send(mailbox *mbox, int result, int move_no, int positions_explored)
{
  // waitting for the signal when there is a space available in the mailbox data
  multiprocessor_wait(mbox->space_available);

  // waiting for the a singal when in the mailbox parent is aviable
  multiprocessor_wait(mbox->mutex);

  // setting up semaphores
  // copying the parameters into the field
  // putting the data into the array within the mailbox
  mbox->data[mbox->in].result = result;
  mbox->data[mbox->in].move_no = move_no;
  mbox->data[mbox->in].positions_explored = positions_explored;
  // incrementing the index, adding one into the mbox in, dividing by MAX_MAILBOX_DATA
  mbox->in = (mbox->in + 1) % MAX_MAILBOX_DATA;

  // send a signal to the parent that mailbox can recieve the data
  multiprocessor_signal(mbox->mutex);
  // send a signal that the new item ready to read in the mailbox
  multiprocessor_signal(mbox->item_available);
}

// mailbox_rec its a consumer that has a shared buffer.
// it will need to retrieve specific data, result, move number, abd position explored from the shared buffer

// pointer to the mail box, reciving result with pointer,
// ponter to the move number so the program knows which move was completed,
// number of positon that the child has explorerd
void mailbox_rec(mailbox *mbox,
                 int *result, int *move_no, int *positions_explored)
{

  // waiting for a signal when there is a data in the mailbox ready to be read
  multiprocessor_wait(mbox->item_available);

  // waiting for a singal when the child is opened in the mailbox
  multiprocessor_wait(mbox->mutex);

  // taking data out of buffer
  // parents reads the data
  *result = mbox->data[mbox->out].result;
  *move_no = mbox->data[mbox->out].move_no;
  *positions_explored = mbox->data[mbox->out].positions_explored;
  // incrementing the index, adding one into the mbox in, dividing by MAX_MAILBOX_DATA
  mbox->out = (mbox->out + 1) % MAX_MAILBOX_DATA;
  // sending a singal to the mailbox that its ready to be open for the child
  multiprocessor_signal(mbox->mutex);
  // sending a singal that there is space available in the mailbox for the data
  multiprocessor_signal(mbox->space_available);
}
