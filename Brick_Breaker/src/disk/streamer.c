#include "streamer.h"
#include "memory/heap/kheap.h"
#include "config.h"
struct disk_stream* diskstreamer_new(int disk_id)
{
    struct disk* disk = disk_get(disk_id);
    if (!disk)
    {
        return 0;
    }

    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int pos)
{
    stream->pos = pos;
    return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total)
{
    int res = 0;

    while (total > 0)
    {
        int sector = stream->pos / PEACHOS_SECTOR_SIZE;
        int offset = stream->pos % PEACHOS_SECTOR_SIZE;

        char buf[PEACHOS_SECTOR_SIZE];
        res = disk_read_block(stream->disk, sector, 1, buf);
        if (res < 0)
            return res;

        int remaining_in_sector = PEACHOS_SECTOR_SIZE - offset;
        int to_copy = (total < remaining_in_sector) ? total : remaining_in_sector;

        memcpy(out, buf + offset, to_copy);

        stream->pos += to_copy;
        out = (char*)out + to_copy;
        total -= to_copy;
    }

    return 0;
}

void diskstreamer_close(struct disk_stream* stream)
{
    kfree(stream);
}