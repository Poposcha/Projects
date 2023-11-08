# -*- coding: utf-8 -*-

import scrapy
import pycountry
from locations.items import GeojsonPointItem
from locations.categories import Code
from typing import List, Dict
import uuid


class HongLeongBank(scrapy.Spider):
    name = 'hong_leong_bank_dac'
    brand_name = 'Hong Leong Bank'
    spider_type: str = 'chain'
    spider_chain_id: str = "7089"
    spider_categories: List[str] = [Code.BANK, Code.ATM]
    spider_countries: List[str] = [pycountry.countries.lookup('my').alpha_2]
    allowed_domains: List[str] = ['hlb.com.my']

    def start_requests(self):
        '''
        Spider entrypoint.
        Request chaining starts from here.
        '''
        url: str = "https://www.hlb.com.my/bin/hlb/branchlocator.json?locale=en&dataPath=%2Fcontent%2Fhlb%2Fmy%2Fbranch-details%2Fbranch-locator&_=1684304078421"
        yield scrapy.Request(
            url=url,
        )

    def parse_opening_hours(self, hours):
        days_of_week = {
            "Mon" : "Mo",
            "Tue" : "Tu",
            "Wed" : "We",
            "Thu" : "Th",
            "Fri" : "Fr",
            "Sat" : "Sa",
            "Sun" : "Su"
        }

        try:
            ind = hours.index("Business Hours")
        except:
            try:
                ind = hours.index(" Business Hours")
            except:
                return ""
       
        for key in days_of_week:
            for i in range(ind + 1, len(hours)):
                hours[i] = hours[i].replace(key, days_of_week[key])

        bus_hours = hours[ind+1:]
        formatted = ""

        for row in bus_hours:
            row = row.replace(":", " ")
            items = row.split(" ")
            for i in range(len(items)):
                if items[i].find("pm") != -1:

                    ind_dot = items[i].find(".")
                    items[i] = str(int(items[i][:ind_dot]) + 12) + ":" + items[i][ind_dot+1:]
                    items[i] = items[i].replace("pm", "")
                if items[i].find("am") != -1:
                    items[i] = items[i].replace("am", "")
                    items[i] = items[i].replace(".", ":")
            items = " ".join(items)
            while items.find("  ") != -1:
                items = items.replace("  ", " ")
            items = items.split(" ")
            for item in items:
                if item != "" and item[-1].isalpha():
                    formatted += item + " "
                elif item!="":
                    formatted += item
            formatted +=  ", "

            while formatted.find(" -") != -1:
                formatted = formatted.replace(" -", "-")
        formatted = formatted[:-2]

        return formatted

    def parse(self, response) -> GeojsonPointItem:
        branches = response.json()["result"]
        for row in branches:
            if "branch_name" in row:
                data = {
                    "brand": self.brand_name,
                    "chain_id": self.spider_chain_id,
                    "chain_name": self.brand_name,
                    "ref": uuid.uuid4().hex,
                    "name": row.get('branch_name'),
                    "addr_full": row.get("address"),
                    "country": row.get("country"),
                    "city": row.get('city'),
                    "street": row.get("street"),
                    "website": "https://www.hlb.com.my/",
                    "opening_hours": self.parse_opening_hours(row.get("business_hours")),
                    "phone": row.get("telephone").replace("-", ""),  
                    "lat": row.get("lat"),
                    "lon": row.get("lng"),
                }
                yield GeojsonPointItem(**data)

