//
// Bareflank Extended APIs
//
// Copyright (C) 2015 Assured Information Security, Inc.
// Author: Rian Quinn        <quinnr@ainfosec.com>
// Author: Brendan Kerrigan  <kerriganb@ainfosec.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef EPT_INTEL_X64_H
#define EPT_INTEL_X64_H

#include <gsl/gsl>

#include <vector>
#include <memory>

#include <memory.h>
#include <vmcs/ept_entry_intel_x64.h>

class ept_intel_x64
{
public:

    using pointer = uintptr_t *;
    using integer_pointer = uintptr_t;
    using size_type = std::size_t;
    using index_type = uint64_t;
    using memory_descriptor_list = std::vector<memory_descriptor>;

    /// Constructor
    ///
    /// Creates a extended page table, and stores the parent entry that points
    /// to this entry so that you can modify the properties of this extended
    /// page table as needed.
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param epte the parent extended page table entry that points to this
    ///     table
    ///
    ept_intel_x64(pointer epte = nullptr);

    /// Destructor
    ///
    /// @expects none
    /// @ensures none
    ///
    ~ept_intel_x64() = default;

    ///
    /// Get an EPT entry
    ///
    /// Retrieves the EPT entry at the given index.
    ///
    /// @expects none
    /// @ensures none
    //
    /// @param index the index of the entry to retrieve.
    /// @return an entry object constructed from the data at @param index.
    ///
    ept_entry_intel_x64 get_entry(index_type index);

    /// Add Page (1G Granularity)
    ///
    /// Adds a page to the extended page table structure. Note that this is the
    /// public function, and should only be used to add pages to the
    /// PML4 extended page table. This function will call a private version that
    /// will parse through the different levels making sure the guest physical
    /// address provided is valid.
    ///
    /// If the gpa maps to a table entry, an exception is thrown.
    ///
    /// If the gpa maps to a page entry with higher granularity than 1G,
    /// an exception is thrown.
    ///
    /// If the gpa maps to a page entry with 1G granularity, it returns the
    /// entry unmodified.
    ///
    /// Otherwise, it returns an entry with only bit 7 set
    /// (so the entry maps a 1G page).
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param gpa the guest physical address of the page to add
    /// @return the resulting epte.
    ///
    ept_entry_intel_x64 add_page_1g(integer_pointer gpa)
    { return add_page(gpa, intel_x64::ept::pml4::from, intel_x64::ept::pdpt::from); }

    /// Add Page (2M Granularity)
    ///
    /// Adds a page to the extended page table structure. Note that this is the
    /// public function, and should only be used to add pages to the
    /// PML4 extended page table. This function will call a private version that
    /// will parse through the different levels making sure the guest physical
    /// address provided is valid.
    ///
    /// If the gpa maps to a table entry, an exception is thrown.
    ///
    /// If the gpa maps to a page entry with higher granularity than 2M,
    /// an exception is thrown.
    ///
    /// If the gpa maps to a page entry with 2M granularity, it returns the
    /// entry unmodified.
    ///
    /// Otherwise, it returns an entry with only bit 7 set
    /// (so the entry maps a 2M page).
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param gpa the guest physical address of the page to add
    /// @return the resulting epte.
    ///
    ept_entry_intel_x64 add_page_2m(integer_pointer gpa)
    { return add_page(gpa, intel_x64::ept::pml4::from, intel_x64::ept::pd::from); }

    /// Add Page (4K Granularity)
    ///
    /// Adds a page to the extended page table structure. Note that this is the
    /// public function, and should only be used to add pages to the
    /// PML4 extended page table. This function will call a private version that
    /// will parse through the different levels making sure the guest physical
    /// address provided is valid.
    ///
    /// If the gpa maps to a page entry with higher granularity than 4K,
    /// an exception is thrown.
    ///
    /// If the gpa maps to a page entry with 4K granularity, it returns the
    /// entry unmodified.
    ///
    /// Otherwise, it returns an entry with only bit 7 set
    /// (so the entry maps a 4K page).
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param gpa the guest physical address of the page to add
    /// @return the resulting epte.
    ///
    ept_entry_intel_x64 add_page_4k(integer_pointer gpa)
    { return add_page(gpa, intel_x64::ept::pml4::from, intel_x64::ept::pt::from); }

    /// Remove Page
    ///
    /// Removes a page from the extended page table. Note that this function
    /// cleans up as it goes, removing empty extended page tables if they are
    /// detected. For this reason, this operation can be expensive if
    /// mapping / unmapping occurs side by side with addresses that are similar
    /// (extended page tables will be needlessly removed)
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param gpa the guest physical address of the page to remove
    ///
    void remove_page(integer_pointer gpa)
    { remove_page(gpa, intel_x64::ept::pml4::from); }

    /// Find Extended Page Table Entry
    ///
    /// Locates an EPTE given a previously added address.
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param gpa the guest physical address of the page to lookup
    ///
    ept_entry_intel_x64 gpa_to_epte(integer_pointer gpa) const
    { return gpa_to_epte(gpa, intel_x64::ept::pml4::from); }

    /// Extended Page Table to Memory Descriptor List
    ///
    /// This function converts the internal page table tree structure into a
    /// linear, memory descriptor list. Page table entry information is not
    /// provide, only the page tables.
    /// pages.
    ///
    /// @expects
    /// @ensures
    ///
    /// @return memory descriptor list
    ///
    memory_descriptor_list ept_to_mdl() const
    { memory_descriptor_list mdl; return ept_to_mdl(mdl); }

private:

    ept_entry_intel_x64 add_page(integer_pointer gpa, integer_pointer bits, integer_pointer end);
    void remove_page(integer_pointer gpa, integer_pointer bits);
    ept_entry_intel_x64 gpa_to_epte(integer_pointer gpa, integer_pointer bits) const;
    memory_descriptor_list ept_to_mdl(memory_descriptor_list &mdl) const;

    bool empty() const noexcept;
    size_type global_size() const noexcept;
    size_type global_capacity() const noexcept;

private:

    friend class eapis_ut;

    std::unique_ptr<integer_pointer[]> m_ept;
    std::vector<std::unique_ptr<ept_intel_x64>> m_epts;

public:

    ept_intel_x64(ept_intel_x64 &&) noexcept = default;
    ept_intel_x64 &operator=(ept_intel_x64 &&) noexcept = default;

    ept_intel_x64(const ept_intel_x64 &) = delete;
    ept_intel_x64 &operator=(const ept_intel_x64 &) = delete;
};

#endif
