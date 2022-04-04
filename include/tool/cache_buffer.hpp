/** @file cache_buffer.h/hpp
 * @dir app
 * @brief task and its class. It will run g
 * @author Tiger3018
 * @date 2022-03-07
 */
#pragma once
#ifndef __CACHE_BUFFER_H__
#define __CACHE_BUFFER_H__

// include

#include <cstdint>
#include <cstring>
#include <functional>

// definition

// variable

// function and class
/**
 * @param cache_size_ indicates what bytes' length buffer.
 */
template <uint32_t cache_size_>
class CacheBuffer
{
  uint8_t buffer_[cache_size_];
  uint32_t write_pos_ = 0, read_pos_ = 0;
  bool circle_ = 0, error_ = 0;
  uint8_t read_single(void)
  {
    while (read_wait_pos())
      ;
    if (read_pos_ + 1 == cache_size_) {
      read_pos_ = 0;
      circle_ = 0;
      return buffer_[cache_size_ - 1];
    } else {
      return buffer_[read_pos_++];
    }
  }
  bool write_single(uint8_t para_char)
  {
    if (error_ || (write_pos_ + 1 == read_pos_)) {
      error_ = 1;
      return false;
    }
    buffer_[write_pos_++] = para_char;
    if (write_pos_ == cache_size_) {
      write_pos_ = 0;
      circle_ = 1;
    }
    return true;
  }
  bool read_wait_pos(void)
  {
    return !(circle_ ? (1 /*write_pos_ <= read_pos_*/) : (write_pos_ > read_pos_));
  }

  /** not a block method.
     */
  uint8_t read_max_len_change_pos(uint8_t expected_len)
  {
    static uint8_t tmp_len;
    if (circle_) {
      if ((tmp_len = cache_size_ - read_pos_) > expected_len) {
        read_pos_ += expected_len;
        return expected_len;
      } else {
        read_pos_ = 0;
        circle_ = 0;
        return tmp_len;
      }
    } else {
      if ((tmp_len = write_pos_ - read_pos_) > expected_len) {
        read_pos_ += expected_len;
        return expected_len;
      } else {
        read_pos_ = write_pos_;
        return tmp_len;
      }
    }
  }

public:
  constexpr uint8_t & operator[](int32_t sub) { return buffer_[sub]; }
  bool write(uint8_t * para_buf, uint32_t len)
  {
    if (circle_) {
      if (read_pos_ - write_pos_ > len) {
        memcpy(buffer_ + write_pos_, para_buf, len);
        write_pos_ += len;
        return true;
      } else {
        error_ = 1;
        return false;
      }
    } else {
      uint32_t tmp_len = cache_size_ - write_pos_;
      if (tmp_len > len) {
        memcpy(buffer_ + write_pos_, para_buf, len);
        write_pos_ += len;
        return true;
      } else {
        memcpy(buffer_ + write_pos_, para_buf, tmp_len);
        write_pos_ = 0;
        circle_ = 1;
        return write(para_buf + tmp_len, len - tmp_len);
      }
    }
  }
  uint32_t read(uint8_t * para_buf, uint32_t len, std::function<bool()> time_check_callback)
  {
    uint32_t filled_len = 0, max_len = 0;
    do {
      uint8_t * buffer_at_read = buffer_ + read_pos_;
      while (read_wait_pos()) {
        if (!time_check_callback()) {
          return filled_len;
        }
      }
      max_len = read_max_len_change_pos(len - filled_len);
      memcpy(para_buf + filled_len, buffer_at_read, max_len);
      filled_len += max_len;
    } while (filled_len != len);
    return filled_len;
  }
};

//template class CacheBuffer<10>;

#endif /* __CACHE_BUFFER_HPP__ */